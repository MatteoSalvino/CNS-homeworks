#include <openssl/evp.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>

#define BLOCK_SIZE 16
/* to compile this file use the following command : 
   gcc -g -o task task.c `pkg-config --libs openssl` `libgcrypt-config --libs` */

void printHexadecimal(unsigned char* plaintext, int length){
  int i = 0;

  while(i < length){
    printf("%x ", plaintext[i]);
    i++;
  }
  printf("\n");
}

void errorsHandler(void){
    ERR_print_errors_fp(stderr);
    exit(0);
}

//OpenSSL encryption
int encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key,
	    unsigned char* iv, unsigned char* ciphertext){

  EVP_CIPHER_CTX* context;
  int len;
  int ciphertext_len;
  
  //Setting up new cipher context
  if(!(context = EVP_CIPHER_CTX_new()))
    errorsHandler();

  //Initialize encryption operation
  if(EVP_EncryptInit_ex(context, EVP_aes_128_ecb(), NULL, key, iv) == 0)
    errorsHandler();

  //Lets give our plaintext to the cipher
  if(EVP_EncryptUpdate(context, ciphertext, &len, plaintext, plaintext_len) == 0)
    errorsHandler();
  ciphertext_len = len;
  
  //Last plaintext block may be partial, so we will encrypt it
  if(EVP_EncryptFinal_ex(context, ciphertext + len, &len) == 0)
    errorsHandler();
  ciphertext_len += len;

  //Remove cipher context
  EVP_CIPHER_CTX_free(context);

  return ciphertext_len;
}

//Libgcrypt decryption
void decrypt(char* key, int cipher, int mode, char* iv, char* filename_in, char* filename_out,
	     int padding_offset, int padding_bytes){

  gcry_cipher_hd_t context;
  gcry_error_t error;
 

  error = gcry_cipher_open(&context, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);
  
  if(gcry_err_code(error))
    printf("Some error occurs.\n");
  
  gcry_cipher_setkey(context, key, BLOCK_SIZE);
  gcry_cipher_setiv(context, iv, BLOCK_SIZE);

  FILE* file_in = fopen(filename_in, "rb");
  FILE* file_out = fopen(filename_out, "wb");
  
  char* buffer = (char*) malloc(sizeof(char) * BLOCK_SIZE);
  int counter = 0, bytes_read = 0;
  
  while(! feof(file_in)){
    bytes_read = fread(buffer, 1, BLOCK_SIZE, file_in);
    counter += bytes_read;
    
    error = gcry_cipher_decrypt(context, buffer, BLOCK_SIZE, NULL, 0);
    
    if((counter - padding_bytes) == padding_offset){
      fwrite(buffer, 1, BLOCK_SIZE - padding_bytes, file_out);
      printHexadecimal(buffer, BLOCK_SIZE - padding_bytes);
      break;
    } else {
      fwrite(buffer, 1, BLOCK_SIZE, file_out);
      printHexadecimal(buffer, BLOCK_SIZE);
    }
  }

  fclose(file_in);
  fclose(file_out);
  gcry_cipher_close(context);
}


int main(int argc, char** argv){

  char* key = "1708108";
  char* iv = (char*) malloc(sizeof(char)*BLOCK_SIZE);
  memset(iv, 0, sizeof(iv));
  
  char* filename_in = "./sample_image.jpeg";
  char* filename_enc = "./enc_image";
  char* filename_out = "./res_image.jpeg";

 
  FILE* file_in = fopen(filename_in, "rb");
  fseek(file_in, 0L, SEEK_END);
  int padding_offset = ftell(file_in);
  rewind(file_in);
  
  int padding_bytes = padding_offset % BLOCK_SIZE;
  unsigned char* plaintext = (unsigned char*) malloc(sizeof(unsigned char) * (padding_offset + BLOCK_SIZE));

  int bytes_read = 0;
  int res = 0;
  while(! feof(file_in)){
     res = fread(plaintext + bytes_read, 1, BLOCK_SIZE, file_in);
     bytes_read += res;
  }
  fclose(file_in);
  printHexadecimal(plaintext, bytes_read);
  
  //OpenSSL encryption
  unsigned char ciphertext[padding_offset];
  int ciphertext_len = encrypt(plaintext, padding_offset, key, iv, ciphertext);

  FILE* file_enc = fopen(filename_enc, "wb");
  int written_bytes = 0;
  
  while(written_bytes < ciphertext_len){
    fwrite(ciphertext + written_bytes, 1, BLOCK_SIZE, file_enc);
    written_bytes += BLOCK_SIZE;
  }
  fclose(file_enc);
  
  
  //Libgcrypt decryption
  decrypt(key, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_CBC, iv, filename_enc, filename_out, padding_offset, padding_bytes);
  
  return 0;
}
