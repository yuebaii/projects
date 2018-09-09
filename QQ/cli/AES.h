#include <iostream>
#include <string>
#include "aes.h"
#include "base64.h"


string DecryptionAES(const string& strSrc,const char *g_key); //AES解密
string EncryptionAES(const string& strSrc, const char*g_key); //AES加密
