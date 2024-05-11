#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <cinttypes>

#include <openssl/evp.h>

class Algorithm{

    public:

        Algorithm(const std::string& name){

            md = EVP_get_digestbyname(name.c_str());

            if(md == nullptr) throw std::runtime_error(std::string("Can't load algorithm \"") + name + "\"");

            ctx = EVP_MD_CTX_new();

            EVP_DigestInit(ctx, md);

        }


        ~Algorithm(){

            EVP_MD_CTX_free(ctx);

        }


        void update(const char* data, size_t length){

            EVP_DigestUpdate(ctx, data, length);

        }


        void finish(std::vector<uint8_t>& digest){

            digest.resize(EVP_MD_size(md));

            EVP_DigestFinal(ctx, digest.data(), nullptr);

        }

    private:

        const EVP_MD* md = nullptr;
        EVP_MD_CTX* ctx = nullptr;

};