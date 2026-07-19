#include <QString>
#include <QtTest>
#include <QBuffer>
#include <QColorSpace>
#include <QCoreApplication>
#include <QFile>
#include <QImageReader>
#include <QMovie>
#include <QtEndian>
#include <zlib.h>

class ApngPluginTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testFormats();

	void testImageReading_data();
	void testImageReading();
	void testAnimation_data();
	void testAnimation();
	void testFrameDelays();
	void testColorSpace();
};

void ApngPluginTest::testFormats()
{
	QVERIFY(QImageReader::supportedImageFormats().contains("apng"));
	QVERIFY(QMovie::supportedFormats().contains("apng"));
}

void ApngPluginTest::testImageReading_data()
{
	QTest::addColumn<QString>("path");
	QTest::addColumn<bool>("valid");
	QTest::addColumn<QSize>("size");
	QTest::addColumn<QPoint>("controlPixel");
	QTest::addColumn<QColor>("controlPixelColor");

	QTest::newRow("sample-1") << QStringLiteral(":/testdata/sample-1.apng")
							  << true
							  << QSize(320, 240)
							  << QPoint(211, 25)
							  << QColor(0xe8, 0x00, 0x00);

	QTest::newRow("sample-2") << QStringLiteral(":/testdata/sample-2.apng")
							  << true
							  << QSize(150, 150)
							  << QPoint(74, 34)
							  << QColor(0xd4, 0x00, 0x00);

	QTest::newRow("sample-3") << QStringLiteral(":/testdata/sample-3.apng")
							  << true
							  << QSize(150, 150)
							  << QPoint(74, 34)
							  << QColor(0xd4, 0x00, 0x00);

	QTest::newRow("sample-4") << QStringLiteral(":/testdata/sample-4.apng")
							  << false
							  << QSize()
							  << QPoint()
							  << QColor();
}

void ApngPluginTest::testImageReading()
{
	QFETCH(QString, path);
	QFETCH(bool, valid);
	QFETCH(QSize, size);
	QFETCH(QPoint, controlPixel);
	QFETCH(QColor, controlPixelColor);

	QImageReader reader(path, "apng");

	QCOMPARE(reader.canRead(), valid);

	if(!valid)//test ends here for invalid
		return;

	QCOMPARE(reader.format(), QByteArray("apng"));
	auto image = reader.read();
	QVERIFY(!image.isNull());
	QCOMPARE(image.size(), size);
	QCOMPARE(image.pixelColor(controlPixel), controlPixelColor);
}

void ApngPluginTest::testAnimation_data()
{
	QTest::addColumn<QString>("path");
	QTest::addColumn<bool>("animated");
	QTest::addColumn<int>("frames");
	QTest::addColumn<int>("frame0Delay");
	QTest::addColumn<int>("loops");

	QTest::newRow("sample-1") << QStringLiteral(":/testdata/sample-1.apng")
							  << true
							  << 101
							  << 30
							  << -1;

	QTest::newRow("sample-2") << QStringLiteral(":/testdata/sample-2.apng")
							  << true
							  << 4
							  << 500
							  << -1;

	QTest::newRow("sample-3") << QStringLiteral(":/testdata/sample-3.apng")
							  << false
							  << 1
							  << 0
							  << 0;
}

void ApngPluginTest::testAnimation()
{
	QFETCH(QString, path);
	QFETCH(bool, animated);
	QFETCH(int, frames);
	QFETCH(int, frame0Delay);
	QFETCH(int, loops);

	QImageReader reader(path, "apng");

	//test general stuff
	QCOMPARE(reader.supportsAnimation(), animated);
	QCOMPARE(reader.imageCount(), frames);
	QCOMPARE(reader.currentImageNumber(), 0);
	QCOMPARE(reader.nextImageDelay(), 0);
	QCOMPARE(reader.loopCount(), loops);

	if(!animated)
		return;

	//test frame jumping
	QVERIFY(reader.jumpToNextImage());
	QCOMPARE(reader.nextImageDelay(), frame0Delay);
	QVERIFY(reader.jumpToImage(frames - 1));
	QVERIFY(!reader.jumpToNextImage());
	QVERIFY(!reader.jumpToImage(frames));
	QVERIFY(!reader.jumpToImage(-1));
	QVERIFY(reader.jumpToImage(0));

	//test incremental reading
	QImage image;
	auto cnt = 0;
	forever {
		image = reader.read();
		if(image.isNull())
			break;
		auto c = ++cnt;
		if(c >= frames)
			c = -1;
		QCOMPARE(reader.currentImageNumber(), c);
	};
	QCOMPARE(cnt, frames);
}

void ApngPluginTest::testFrameDelays()
{
	QImageReader reader(QStringLiteral(":/testdata/sample-5.apng"), "apng");

	//test general stuff
	QVERIFY(reader.supportsAnimation());
	QCOMPARE(reader.imageCount(), 4);
	QCOMPARE(reader.currentImageNumber(), 0);
	QCOMPARE(reader.nextImageDelay(), 0);
	QCOMPARE(reader.loopCount(), -1);

	QList<QPair<int, bool>> delays {
		{2000, true},
		{500, true},
		{1000, true},
		{500, false},
		{0, false}
	};
	while(!delays.isEmpty()) {
		const auto dInfo = delays.takeFirst();
		QCOMPARE(reader.jumpToNextImage(), dInfo.second);
		QCOMPARE(reader.nextImageDelay(), dInfo.first);
	}
}

void ApngPluginTest::testColorSpace()
{
	QFile file(QStringLiteral(":/testdata/sample-2.apng"));
	QVERIFY(file.open(QIODevice::ReadOnly));
	QByteArray apng = file.readAll();

	const QColorSpace expectedColorSpace{QColorSpace::AdobeRgb};
	const QByteArray profile = expectedColorSpace.iccProfile();
	QVERIFY(!profile.isEmpty());

	uLongf compressedSize = compressBound(static_cast<uLong>(profile.size()));
	QByteArray compressed;
	compressed.resize(static_cast<int>(compressedSize));
	QCOMPARE(compress2(reinterpret_cast<Bytef *>(compressed.data()), &compressedSize,
					   reinterpret_cast<const Bytef *>(profile.constData()),
					   static_cast<uLong>(profile.size()), Z_BEST_COMPRESSION),
			 Z_OK);
	compressed.resize(static_cast<int>(compressedSize));

	QByteArray chunkData{"Adobe RGB"};
	chunkData.append('\0');
	chunkData.append('\0'); // PNG compression method
	chunkData.append(compressed);

	QByteArray chunk;
	chunk.resize(4);
	qToBigEndian<quint32>(static_cast<quint32>(chunkData.size()),
						 reinterpret_cast<uchar *>(chunk.data()));
	chunk.append("iCCP", 4);
	chunk.append(chunkData);

	uLong checksum = crc32(0L, Z_NULL, 0);
	checksum = crc32(checksum,
					 reinterpret_cast<const Bytef *>(chunk.constData() + 4),
					 static_cast<uInt>(chunk.size() - 4));
	const auto oldSize = chunk.size();
	chunk.resize(oldSize + 4);
	qToBigEndian<quint32>(static_cast<quint32>(checksum),
						 reinterpret_cast<uchar *>(chunk.data() + oldSize));

	// Insert after the PNG signature and IHDR chunk, before the APNG chunks.
	apng.insert(33, chunk);
	QBuffer buffer{&apng};
	QVERIFY(buffer.open(QIODevice::ReadOnly));

	QImageReader reader{&buffer, "apng"};
	const QColorSpace parsedColorSpace = QColorSpace::fromIccProfile(profile);
	QVERIFY(parsedColorSpace.isValid());

	const QImage firstFrame = reader.read();
	QVERIFY(!firstFrame.isNull());
	QCOMPARE(firstFrame.colorSpace(), parsedColorSpace);

	const QImage secondFrame = reader.read();
	QVERIFY(!secondFrame.isNull());
	QCOMPARE(secondFrame.colorSpace(), parsedColorSpace);
}

QTEST_MAIN(ApngPluginTest)

#include "tst_apngplugin.moc"
