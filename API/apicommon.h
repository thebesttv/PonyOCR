#ifndef APIURL_H
#define APIURL_H

#include <QString>
#include <QMap>

namespace baidu {
const QString ACCESS_TOKEN("https://aip.baidubce.com/oauth/2.0/token");
// 通用文字识别
const QString GENERAL_BASIC("https://aip.baidubce.com/rest/2.0/ocr/v1/general_basic");
// 通用文字识别(高精度)
const QString ACCURATE_BASIC("https://aip.baidubce.com/rest/2.0/ocr/v1/accurate_basic");
// 通用文字识别(含位置)
const QString GENERAL("https://aip.baidubce.com/rest/2.0/ocr/v1/general");
// 通用文字识别(高精度,含位置)
const QString ACCURATE("https://aip.baidubce.com/rest/2.0/ocr/v1/accurate");
// 网络图片
const QString WEB_IMAGE("https://aip.baidubce.com/rest/2.0/ocr/v1/webimage");
// 表格识别(异步)
const QString TABLE_REQUEST("https://aip.baidubce.com/rest/2.0/solution/v1/form_ocr/request");
const QString TABLE_RESULT("https://aip.baidubce.com/rest/2.0/solution/v1/form_ocr/get_request_result");
// 二维码
const QString QRCODE("https://aip.baidubce.com/rest/2.0/ocr/v1/qrcode");
// 数字
const QString NUMBERS("https://aip.baidubce.com/rest/2.0/ocr/v1/numbers");
// 手写
const QString HANDWRITING("https://aip.baidubce.com/rest/2.0/ocr/v1/handwriting");
// 公式
const QString FORMULA("https://aip.baidubce.com/rest/2.0/ocr/v1/formula");

extern QMap<int, QString> ErrorCodeMap;
}

namespace tencent {

const QString GENERAL("https://api.ai.qq.com/fcgi-bin/ocr/ocr_generalocr");
const QString HANDWRITING("https://api.ai.qq.com/fcgi-bin/ocr/ocr_handwritingocr");

extern QMap<int, QString> ErrorCodeMap;
}

namespace ocrspace {

const QString GENERAL("https://api.ocr.space/parse/image");

extern QMap<int, QString> ErrorCodeMap;

}

namespace mathpix {

const QString V3_TEXT("https://api.mathpix.com/v3/text");

}

void GlobalInitAPIResources();

#endif // APIURL_H
