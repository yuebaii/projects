
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
std::string EncodeAES( const std::string& password, const std::string& data );
void aes_box_decrypt(unsigned  char* source_string, unsigned char* des_string) ;
void aes_box_encrypt(unsigned  char* source_string, unsigned char* des_string);
std::string DecodeAES( const std::string& strPassword, const std::string& strData );


/*
#ifndef OPENSSL_TEST_H
#define OPENSSL_TEST_H
#include <string>

class openssl_test
{
	public:
		openssl_test(std::string Texts, std::string keys);
		std::string des_encrypt();
		std::string des_decrypt();
		//生成密钥对  
		void generateRSAKey(std::string strKey[2]);
		std::string rsa_pub_encrypt(const std::string &clearText, const std::string &pubKey);//clearText 明文 
		std::string rsa_pri_decrypt(const std::string &cipherText, const std::string &priKey);//cipherText 密文
	private:
		std::string mwormw;
		std::string deskey;
}

#endif
*/
