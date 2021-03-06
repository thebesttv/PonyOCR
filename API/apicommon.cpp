﻿#include "apicommon.h"

QMap<int, QString> baidu::ErrorCodeMap;
QMap<int, QString> tencent::ErrorCodeMap;
QMap<int, QString> ocrspace::ErrorCodeMap;

void GlobalInitAPIResources()
{
    // Baidu
    baidu::ErrorCodeMap[1]=QStringLiteral(u"服务器内部错误，请再次请求， 如果持续出现此类错误，请在控制台提交工单联系技术支持团队");
    baidu::ErrorCodeMap[2]=QStringLiteral(u"服务暂不可用，请再次请求， 如果持续出现此类错误，请在控制台提交工单联系技术支持团队");
    baidu::ErrorCodeMap[3]=QStringLiteral(u"调用的API不存在，请检查后重新尝试");
    baidu::ErrorCodeMap[4]=QStringLiteral(u"集群超限额");
    baidu::ErrorCodeMap[6]=QStringLiteral(u"无权限访问该用户数据");
    baidu::ErrorCodeMap[14]=QStringLiteral(u"IAM鉴权失败，建议用户参照文档自查生成sign的方式是否正确，或换用控制台中ak sk的方式调用");
    baidu::ErrorCodeMap[17]=QStringLiteral(u"每天请求量超限额");
    baidu::ErrorCodeMap[18]=QStringLiteral(u"QPS超限额");
    baidu::ErrorCodeMap[19]=QStringLiteral(u"请求总量超限额");
    baidu::ErrorCodeMap[100]=QStringLiteral(u"无效的access_token参数，请检查后重新尝试");
    baidu::ErrorCodeMap[110]=QStringLiteral(u"access_token无效");
    baidu::ErrorCodeMap[111]=QStringLiteral(u"access token过期");
    baidu::ErrorCodeMap[216100]=QStringLiteral(u"请求中包含非法参数，请检查后重新尝试");
    baidu::ErrorCodeMap[216101]=QStringLiteral(u"缺少必须的参数，请检查参数是否有遗漏");
    baidu::ErrorCodeMap[216102]=QStringLiteral(u"请求了不支持的服务，请检查调用的url");
    baidu::ErrorCodeMap[216103]=QStringLiteral(u"请求中某些参数过长，请检查后重新尝试");
    baidu::ErrorCodeMap[216110]=QStringLiteral(u"appid不存在，请重新核对信息是否为后台应用列表中的appid");
    baidu::ErrorCodeMap[216200]=QStringLiteral(u"图片为空，请检查后重新尝试");
    baidu::ErrorCodeMap[216201]=QStringLiteral(u"上传的图片格式错误，现阶段我们支持的图片格式为：PNG、JPG、JPEG、BMP，请进行转码或更换图片");
    baidu::ErrorCodeMap[216202]=QStringLiteral(u"上传的图片大小错误，现阶段我们支持的图片大小为：base64编码后小于4M，分辨率不高于4096*4096，请重新上传图片");
    baidu::ErrorCodeMap[216630]=QStringLiteral(u"识别错误，请再次请求，如果持续出现此类错误，请在控制台提交工单联系技术支持团队");
    baidu::ErrorCodeMap[216631]=QStringLiteral(u"识别银行卡错误，出现此问题的原因一般为：您上传的图片非银行卡正面，上传了异形卡的图片或上传的银行卡正品图片不完整");
    baidu::ErrorCodeMap[216633]=QStringLiteral(u"识别身份证错误，出现此问题的原因一般为：您上传了非身份证图片或您上传的身份证图片不完整");
    baidu::ErrorCodeMap[216634]=QStringLiteral(u"检测错误，请再次请求，如果持续出现此类错误，请在控制台提交工单联系技术支持团队");
    baidu::ErrorCodeMap[282000]=QStringLiteral(u"服务器内部错误，如果您使用的是高精度接口，报这个错误码的原因可能是您上传的图片中文字过多，识别超时导致的，建议您对图片进行切割后再识别，其他情况请再次请求， 如果持续出现此类错误，请在控制台提交工单联系技术支持团队");
    baidu::ErrorCodeMap[282003]=QStringLiteral(u"请求参数缺失");
    baidu::ErrorCodeMap[282005]=QStringLiteral(u"处理批量任务时发生部分或全部错误，请根据具体错误码排查");
    baidu::ErrorCodeMap[282006]=QStringLiteral(u"批量任务处理数量超出限制，请将任务数量减少到10或10以下");
    baidu::ErrorCodeMap[282102]=QStringLiteral(u"未检测到图片中识别目标，请确保图片中包含对应卡证票据");
    baidu::ErrorCodeMap[282103]=QStringLiteral(u"图片目标识别错误，请确保图片中包含对应卡证票据，如果持续出现此类错误，请在控制台提交工单联系技术支持团队");
    baidu::ErrorCodeMap[282110]=QStringLiteral(u"URL参数不存在，请核对URL后再次提交");
    baidu::ErrorCodeMap[282111]=QStringLiteral(u"URL格式非法，请检查url格式是否符合相应接口的入参要求");
    baidu::ErrorCodeMap[282112]=QStringLiteral(u"url下载超时，请检查url对应的图床/图片无法下载或链路状况不好，您可以重新尝试一下，如果多次尝试后仍不行，建议更换图片地址");
    baidu::ErrorCodeMap[282113]=QStringLiteral(u"URL返回无效参数");
    baidu::ErrorCodeMap[282114]=QStringLiteral(u"URL长度超过1024字节或为0");
    baidu::ErrorCodeMap[282808]=QStringLiteral(u"request id xxxxx 不存在");
    baidu::ErrorCodeMap[282809]=QStringLiteral(u"返回结果请求错误（不属于excel或json）");
    baidu::ErrorCodeMap[282810]=QStringLiteral(u"图像识别错误");

    // Tencent
    tencent::ErrorCodeMap[9]=QStringLiteral(u"qps超过限制\n用户认证升级或者降低调用频率");
    tencent::ErrorCodeMap[4096]=QStringLiteral(u"参数非法\n请检查请求参数是否符合要求");
    tencent::ErrorCodeMap[12289]=QStringLiteral(u"应用不存在\n请检查app_id是否有效的应用标识（AppId）");
    tencent::ErrorCodeMap[12801]=QStringLiteral(u"素材不存在\n请检查app_id对应的素材模版id");
    tencent::ErrorCodeMap[12802]=QStringLiteral(u"素材ID与应用ID不匹配\n请检查app_id对应的素材模版id");
    tencent::ErrorCodeMap[16385]=QStringLiteral(u"缺少app_id参数\n请检查请求中是否包含有效的app_id参数");
    tencent::ErrorCodeMap[16386]=QStringLiteral(u"缺少time_stamp参数\n请检查请求中是否包含有效的time_stamp参数");
    tencent::ErrorCodeMap[16387]=QStringLiteral(u"缺少nonce_str参数\n请检查请求中是否包含有效的nonce_str参数");
    tencent::ErrorCodeMap[16388]=QStringLiteral(u"请求签名无效\n请检查请求中的签名信息（sign）是否有效");
    tencent::ErrorCodeMap[16389]=QStringLiteral(u"缺失API权限\n请检查应用是否勾选当前API所属接口的权限");
    tencent::ErrorCodeMap[16390]=QStringLiteral(u"time_stamp参数无效\n请检查time_stamp距离当前时间是否超过5分钟");
    tencent::ErrorCodeMap[16391]=QStringLiteral(u"同义词识别结果为空\n请尝试更换文案");
    tencent::ErrorCodeMap[16392]=QStringLiteral(u"专有名词识别结果为空\n请尝试更换文案");
    tencent::ErrorCodeMap[16393]=QStringLiteral(u"意图识别结果为空\n请尝试更换文案");
    tencent::ErrorCodeMap[16394]=QStringLiteral(u"闲聊返回结果为空\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16396]=QStringLiteral(u"图片格式非法\n请检查图片格式是否符合API要求");
    tencent::ErrorCodeMap[16397]=QStringLiteral(u"图片体积过大\n请检查图片大小是否超过API限制");
    tencent::ErrorCodeMap[16402]=QStringLiteral(u"图片没有人脸\n请检查图片是否包含人脸");
    tencent::ErrorCodeMap[16403]=QStringLiteral(u"相似度错误\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16404]=QStringLiteral(u"人脸检测失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16405]=QStringLiteral(u"图片解码失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16406]=QStringLiteral(u"特征处理失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16407]=QStringLiteral(u"提取轮廓错误\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16408]=QStringLiteral(u"提取性别错误\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16409]=QStringLiteral(u"提取表情错误\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16410]=QStringLiteral(u"提取年龄错误\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16411]=QStringLiteral(u"提取姿态错误\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16412]=QStringLiteral(u"提取眼镜错误\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16413]=QStringLiteral(u"提取魅力值错误\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16414]=QStringLiteral(u"语音合成失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16415]=QStringLiteral(u"图片为空\n请检查图片是否正常");
    tencent::ErrorCodeMap[16416]=QStringLiteral(u"个体已存在\n请检查个体是否已存在");
    tencent::ErrorCodeMap[16417]=QStringLiteral(u"个体不存在\n请检查个体是否不存在");
    tencent::ErrorCodeMap[16418]=QStringLiteral(u"人脸不存在\n请检查人脸是否不存在");
    tencent::ErrorCodeMap[16419]=QStringLiteral(u"分组不存在\n请检查分组是否不存在");
    tencent::ErrorCodeMap[16420]=QStringLiteral(u"分组列表不存在\n请检查分组列表是否不存在");
    tencent::ErrorCodeMap[16421]=QStringLiteral(u"人脸个数超过限制\n请检查是否超过系统限制");
    tencent::ErrorCodeMap[16422]=QStringLiteral(u"个体个数超过限制\n请检查是否超过系统限制");
    tencent::ErrorCodeMap[16423]=QStringLiteral(u"组个数超过限制\n请检查是否超过系统限制");
    tencent::ErrorCodeMap[16424]=QStringLiteral(u"对个体添加了几乎相同的人脸\n请检查个体已添加的人脸");
    tencent::ErrorCodeMap[16425]=QStringLiteral(u"无效的图片格式\n请检查图片格式是否符号API要求");
    tencent::ErrorCodeMap[16426]=QStringLiteral(u"图片模糊度检测失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16427]=QStringLiteral(u"美食图片检测失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16428]=QStringLiteral(u"提取图像指纹失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16429]=QStringLiteral(u"图像特征比对失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16430]=QStringLiteral(u"OCR照片为空\n请检查待处理图片是否为空");
    tencent::ErrorCodeMap[16431]=QStringLiteral(u"OCR识别失败\n请尝试更换带有文字的图片");
    tencent::ErrorCodeMap[16432]=QStringLiteral(u"输入图片不是身份证\n请检查图片是否为身份证");
    tencent::ErrorCodeMap[16433]=QStringLiteral(u"名片无足够文本\n请检查名片是否正常");
    tencent::ErrorCodeMap[16434]=QStringLiteral(u"名片文本行倾斜角度太大\n请检查名片是否正常");
    tencent::ErrorCodeMap[16435]=QStringLiteral(u"名片模糊\n请检查名片是否正常");
    tencent::ErrorCodeMap[16436]=QStringLiteral(u"名片姓名识别失败\n请尝试更换姓名显示清晰的名片图片");
    tencent::ErrorCodeMap[16437]=QStringLiteral(u"名片电话识别失败\n请尝试更换电话显示清晰的名片图片");
    tencent::ErrorCodeMap[16438]=QStringLiteral(u"图像为非名片图像\n请尝试更换有效的名片图片");
    tencent::ErrorCodeMap[16439]=QStringLiteral(u"检测或者识别失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16440]=QStringLiteral(u"未检测到身份证\n请对准边框(避免拍摄时倾角和旋转角过大、摄像头)");
    tencent::ErrorCodeMap[16441]=QStringLiteral(u"请使用第二代身份证件进行扫描\n请使用第二代身份证进行处理");
    tencent::ErrorCodeMap[16442]=QStringLiteral(u"不是身份证正面照片\n请使用带证件照的一面进行处理");
    tencent::ErrorCodeMap[16443]=QStringLiteral(u"不是身份证反面照片\n请使用身份证反面进行进行处理");
    tencent::ErrorCodeMap[16444]=QStringLiteral(u"证件图片模糊\n请确保证件图片清晰");
    tencent::ErrorCodeMap[16445]=QStringLiteral(u"请避开灯光直射在证件表面\n请避开灯光直射在证件表面");
    tencent::ErrorCodeMap[16446]=QStringLiteral(u"行驾驶证OCR识别失败\n请尝试更换有效的行驾驶证图片");
    tencent::ErrorCodeMap[16447]=QStringLiteral(u"通用OCR识别失败\n请尝试更换带有文字的图片");
    tencent::ErrorCodeMap[16448]=QStringLiteral(u"银行卡OCR预处理错误\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16449]=QStringLiteral(u"银行卡OCR识别失败\n请尝试更换有效的银行卡图片");
    tencent::ErrorCodeMap[16450]=QStringLiteral(u"营业执照OCR预处理失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16451]=QStringLiteral(u"营业执照OCR识别失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16452]=QStringLiteral(u"意图识别超时\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16453]=QStringLiteral(u"闲聊处理超时\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16454]=QStringLiteral(u"语音识别解码失败\n请检查语音参数是否正确编码");
    tencent::ErrorCodeMap[16455]=QStringLiteral(u"语音过长或空\n请检查语音参数是否正确编码或者长度是否合法");
    tencent::ErrorCodeMap[16456]=QStringLiteral(u"翻译引擎失败\n请联系客服反馈问题");
    tencent::ErrorCodeMap[16457]=QStringLiteral(u"不支持的翻译类型\n请检查翻译类型参数是否合法");
    tencent::ErrorCodeMap[16460]=QStringLiteral(u"输入图片与识别场景不匹配\n请检查场景参数是否正确，所传图片与场景是否匹配");
    tencent::ErrorCodeMap[16461]=QStringLiteral(u"识别结果为空\n当前图片无法匹配已收录的标签，请尝试更换图片");
    tencent::ErrorCodeMap[16462]=QStringLiteral(u"多人脸检测识别结果为空\n图片中识别不出人脸，请尝试更换图片");
    tencent::ErrorCodeMap[16467]=QStringLiteral(u"跨年龄人脸识别出错\n请尝试更换有人脸的图片");
    tencent::ErrorCodeMap[16468]=QStringLiteral(u"跨年龄人脸识别结果为空\n源图片与目标图片中识别不出匹配的人脸，请尝试更换图片");
    tencent::ErrorCodeMap[16472]=QStringLiteral(u"音频鉴黄识别出错\n请确保音频地址能正常下载音频，尝试更换音频");

    // OCR.Space
    ocrspace::ErrorCodeMap[1] = QStringLiteral(u"Parsed Successfully\nImage / All pages parsed successfully");
    ocrspace::ErrorCodeMap[2] = QStringLiteral(u"Parsed Partially\nOnly few pages out of all the pages parsed successfully");
    ocrspace::ErrorCodeMap[3] = QStringLiteral(u"Image / All the PDF pages failed parsing\nThis happens mainly because the OCR engine fails to parse an image");
    ocrspace::ErrorCodeMap[4] = QStringLiteral(u"Error occurred when attempting to parse\nThis happens when a fatal error occurs during parsing ");
}
