#ifndef HTTP_LOCALCLIENT_H
#define HTTP_LOCALCLIENT_H

#include <HTTPClient.h>

String HTTPGetLocal(const String& path);

String HTTPPostLocal(const String& path, const String& payload);

#endif // HTTP_LOCALCLIENT_H