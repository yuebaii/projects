
//若采用非对称加密算法管理对称算法的密钥，然后用对称加密算法加密数据，这样我们就
//集成了两类加密算法的优点，既实现了加密速度快的优点，又实现了安全方便管理密钥的优点。
/*【客户端再向服务器发数据！】
1.先由服务器产生公钥和私钥。自己保存私钥用来解密，发送公钥到客户端；
  2.接着客户端接受到公钥；
  3.再由客户端用接受到的公钥对密钥进行加密并发送至服务器；
  4.然后用密钥加密消息发送服务器；
  5.服务器用接受到的密钥完成解密消息！！
【服务器向客户端响应！】
*/

//采用des出现某些字符无法解密成功情况？
//采用aes出现固定密钥解密失败或者说是无法生成密钥， 自定义密钥解密出现空串？？

#include "openssl_test.h"

const char g_iv[17] = "gfdertfghjkuyrtg";
// ---- des对称加解密 ---- //    
// 加密 ecb模式    
std::string des_encrypt(const std::string &clearText, const std::string &key)  
{  
    std::string cipherText; // 密文    
  
    DES_cblock keyEncrypt;  
    memset(keyEncrypt, 0, 8);  
  
    // 构造补齐后的密钥    
    if (key.length() <= 8)  
        memcpy(keyEncrypt, key.c_str(), key.length());  
    else  
        memcpy(keyEncrypt, key.c_str(), 8);  
  
    // 密钥置换    
    DES_key_schedule keySchedule;  
    DES_set_key_unchecked(&keyEncrypt, &keySchedule);  
  
    // 循环加密，每8字节一次    
    const_DES_cblock inputText;  
    DES_cblock outputText;  
    std::vector<unsigned char> vecCiphertext;  
    unsigned char tmp[8];  
  
    for (int i = 0; i < clearText.length() / 8; i++)  
    {  
        memcpy(inputText, clearText.c_str() + i * 8, 8);  
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);  
        memcpy(tmp, outputText, 8);  
  
        for (int j = 0; j < 8; j++)  
            vecCiphertext.push_back(tmp[j]);  
    }  
  
    if (clearText.length() % 8 != 0)  
    {  
        int tmp1 = clearText.length() / 8 * 8;  
        int tmp2 = clearText.length() - tmp1;  
        memset(inputText, 0, 8);  
        memcpy(inputText, clearText.c_str() + tmp1, tmp2);  
        // 加密函数    
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);  
        memcpy(tmp, outputText, 8);  
  
        for (int j = 0; j < 8; j++)  
            vecCiphertext.push_back(tmp[j]);  
    }  
  
    cipherText.clear();  
    cipherText.assign(vecCiphertext.begin(), vecCiphertext.end());  
  
    return cipherText;  
}  
  
// 解密 ecb模式    
std::string des_decrypt(const std::string &cipherText, const std::string &key)  
{  
    std::string clearText; // 明文    
  
    DES_cblock keyEncrypt;  
    memset(keyEncrypt, 0, 8);  
  
    if (key.length() <= 8)  
        memcpy(keyEncrypt, key.c_str(), key.length());  
    else  
        memcpy(keyEncrypt, key.c_str(), 8);  
  
    DES_key_schedule keySchedule;  
    DES_set_key_unchecked(&keyEncrypt, &keySchedule);  
  
    const_DES_cblock inputText;  
    DES_cblock outputText;  
    std::vector<unsigned char> vecCleartext;  
    unsigned char tmp[8];  
  
    for (int i = 0; i < cipherText.length() / 8; i++)  
    {  
        memcpy(inputText, cipherText.c_str() + i * 8, 8);  
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);  
        memcpy(tmp, outputText, 8);  
  
        for (int j = 0; j < 8; j++)  
            vecCleartext.push_back(tmp[j]);  
    }  
  
    if (cipherText.length() % 8 != 0)  
    {  
        int tmp1 = cipherText.length() / 8 * 8;  
        int tmp2 = cipherText.length() - tmp1;  
        memset(inputText, 0, 8);  
        memcpy(inputText, cipherText.c_str() + tmp1, tmp2);  
        // 解密函数    
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);  
        memcpy(tmp, outputText, 8);  
  
        for (int j = 0; j < 8; j++)  
            vecCleartext.push_back(tmp[j]);  
    }  
  
    clearText.clear();  
    clearText.assign(vecCleartext.begin(), vecCleartext.end());  
  
    return clearText;  
}  


// ---- rsa非对称加解密 ---- //    
#define KEY_LENGTH  2048               // 密钥长度  
#define PUB_KEY_FILE "pubkey.pem"    // 公钥路径  
#define PRI_KEY_FILE "prikey.pem"    // 私钥路径  
  
// 函数方法生成密钥对   
void generateRSAKey(std::string strKey[2])  
{  
    // 公私密钥对    
    size_t pri_len;  
    size_t pub_len;  
    char *pri_key = NULL;  
    char *pub_key = NULL;  
  
    // 生成密钥对    
    RSA *keypair = RSA_generate_key(KEY_LENGTH, RSA_3, NULL, NULL);  
  
    BIO *pri = BIO_new(BIO_s_mem());  
    BIO *pub = BIO_new(BIO_s_mem());  
  
    PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);  
    PEM_write_bio_RSAPublicKey(pub, keypair);  
  
    // 获取长度    
    pri_len = BIO_pending(pri);  
    pub_len = BIO_pending(pub);  
  
    // 密钥对读取到字符串    
    pri_key = (char *)malloc(pri_len + 1);  
    pub_key = (char *)malloc(pub_len + 1);  
  
    BIO_read(pri, pri_key, pri_len);  
    BIO_read(pub, pub_key, pub_len);  
  
    pri_key[pri_len] = '\0';  
    pub_key[pub_len] = '\0';  
  
    // 存储密钥对    
    strKey[0] = pub_key;  
    strKey[1] = pri_key;  
  
    // 存储到磁盘（这种方式存储的是begin rsa public key/ begin rsa private key开头的）  
    FILE *pubFile = fopen(PUB_KEY_FILE, "w");  
    if (pubFile == NULL)  
    {  
        assert(false);  
        return;  
    }  
    fputs(pub_key, pubFile);  
    fclose(pubFile);  
  
    FILE *priFile = fopen(PRI_KEY_FILE, "w");  
    if (priFile == NULL)  
    {  
        assert(false);  
        return;  
    }  
    fputs(pri_key, priFile);  
    fclose(priFile);  
  
    // 内存释放  
    RSA_free(keypair);  
    BIO_free_all(pub);  
    BIO_free_all(pri);  
  
    free(pri_key);  
    free(pub_key);  
}  
  
// 命令行方法生成公私钥对（begin public key/ begin private key）  
// 找到openssl命令行工具，运行以下  
// openssl genrsa -out prikey.pem 1024   
// openssl rsa - in privkey.pem - pubout - out pubkey.pem  
  
// 公钥加密    
std::string rsa_pub_encrypt(const std::string &clearText, const std::string &pubKey)  
{  
    std::string strRet;  
    RSA *rsa = NULL;  
    BIO *keybio = BIO_new_mem_buf((unsigned char *)pubKey.c_str(), -1);  
    // 此处有三种方法  
    // 1, 读取内存里生成的密钥对，再从内存生成rsa  
    // 2, 读取磁盘里生成的密钥对文本文件，在从内存生成rsa  
    // 3，直接从读取文件指针生成rsa  
    RSA* pRSAPublicKey = RSA_new();  
    rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);  
  
    int len = RSA_size(rsa);  
    char *encryptedText = (char *)malloc(len + 1);  
    memset(encryptedText, 0, len + 1);  
  
    // 加密函数  
    int ret = RSA_public_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);  
    if (ret >= 0)  
        strRet = std::string(encryptedText, ret);  
  
    // 释放内存  
    free(encryptedText);  
    BIO_free_all(keybio);  
    RSA_free(rsa);  
  
    return strRet;  
}  
  
// 私钥解密    
std::string rsa_pri_decrypt(const std::string &cipherText, const std::string &priKey)  
{  
    std::string strRet;  
    RSA *rsa = RSA_new();  
    BIO *keybio;  
    keybio = BIO_new_mem_buf((unsigned char *)priKey.c_str(), -1);  
  
    // 此处有三种方法  
    // 1, 读取内存里生成的密钥对，再从内存生成rsa  
    // 2, 读取磁盘里生成的密钥对文本文件，在从内存生成rsa  
    // 3，直接从读取文件指针生成rsa  
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);  
  
    int len = RSA_size(rsa);  
    char *decryptedText = (char *)malloc(len + 1);  
    memset(decryptedText, 0, len + 1);  
  
    // 解密函数  
    int ret = RSA_private_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);  
    if (ret >= 0)  
        strRet = std::string(decryptedText, ret);  
  
    // 释放内存  
    free(decryptedText);  
    BIO_free_all(keybio);  
    RSA_free(rsa);  
  
    return strRet;  
}  





