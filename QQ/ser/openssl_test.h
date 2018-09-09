
//若采用非对称加密算法管理对称算法的密钥，然后用对称加密算法加密数据，这样我们就集成了两类加密算法的优点，既实现了加密速度快的优点，又实现了安全方便管理密钥的优点。
#include <iostream>    
#include <cassert>  
#include <string>    
#include <vector>    
#include <string.h>
#include "openssl/md5.h"    
#include "openssl/sha.h"    
#include "openssl/des.h"    
#include "openssl/rsa.h"    
#include "openssl/pem.h" 
#include "openssl/aes.h"   

std::string des_encrypt(const std::string &clearText, const std::string &key);
std::string des_decrypt(const std::string &cipherText, const std::string &key);
void generateRSAKey(std::string strKey[2]);
std::string rsa_pub_encrypt(const std::string &clearText, const std::string &pubKey);
std::string rsa_pri_decrypt(const std::string &cipherText, const std::string &priKey);


