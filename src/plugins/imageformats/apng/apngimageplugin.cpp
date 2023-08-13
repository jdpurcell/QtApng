#include "apngimageplugin.h"
#include "apngimagehandler_p.h"
#include "apngreader_p.h"

ApngImagePlugin::ApngImagePlugin(QObject *parent) :
	QImageIOPlugin(parent)
{}

QImageIOPlugin::Capabilities ApngImagePlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
	if (format == "apng")
		return CanRead;

	return { };
}

QImageIOHandler *ApngImagePlugin::create(QIODevice *device, const QByteArray &format) const
{
	if(device && capabilities(device, format).testFlag(CanRead)) {
		auto handler = new ApngImageHandler();
		handler->setDevice(device);
		handler->setFormat(format);
		return handler;
	} else
		return nullptr;
}
