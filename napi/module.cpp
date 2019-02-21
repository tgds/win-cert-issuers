
#include <napi.h>
#include <Windows.h>
#include <Wincrypt.h>

class WinCerts : public Napi::ObjectWrap<WinCerts> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        WinCerts(const Napi::CallbackInfo &info);

    private:
        static Napi::FunctionReference constructor;
        Napi::Value next(const Napi::CallbackInfo &info);
        std::string getCertInfo(const Napi::Env env);
        HCERTSTORE hSystemStore;
	    PCCERT_CONTEXT  pCertContext;
};

Napi::Object WinCerts::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "WinCerts", {
        InstanceMethod("next", &WinCerts::next)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("WinCerts", func);
    return exports;
}

WinCerts::WinCerts(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WinCerts>(info) {
    Napi::Env env = info.Env();
    this->hSystemStore = CertOpenSystemStoreA(0, "MY");
    this->pCertContext = NULL;

    if (!this->hSystemStore) {
        throw Napi::Error::New(env, "Could not open the cert store.");
    }
}

Napi::FunctionReference WinCerts::constructor;

Napi::Value WinCerts::next(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::Object obj = Napi::Object::New(env);

    if (!this->hSystemStore) {
        obj.Set(Napi::String::New(env, "done"), Napi::Boolean::New(env, true));
        return obj;
    }

    if (this->pCertContext = CertEnumCertificatesInStore(this->hSystemStore, this->pCertContext)) {
        obj.Set(Napi::String::New(env, "done"), Napi::Boolean::New(env, false));
        std::string cert = this->getCertInfo(env);
        obj.Set(Napi::String::New(env, "value"), Napi::String::New(env, cert)); 
        return obj;
    } else {
        if(!CertCloseStore(this->hSystemStore, 0)) {
            throw Napi::Error::New(env, "Could not close the store.");
        }
        this->hSystemStore = NULL;
        obj.Set(Napi::String::New(env, "done"), Napi::Boolean::New(env, true));
        return obj;
    }
}

std::string WinCerts::getCertInfo(const Napi::Env env) {
    DWORD cbSize;
    LPSTR pszString;
    std::string str; 

    cbSize = CertNameToStrA(1, &(this->pCertContext->pCertInfo->Issuer), 1, NULL, 0);

    if (!cbSize) {
        throw Napi::Error::New(env, "Could not get the length of the issuer blob.");
    }

    if (1 == cbSize) {
        return "";
    }

    if (!(pszString = (LPSTR)malloc(cbSize * sizeof(TCHAR)))) {
        throw Napi::Error::New(env, "Memory allocation failed.");
    }

    CertNameToStrA(1, &(pCertContext->pCertInfo->Issuer), 1, pszString, cbSize);
    str = pszString;

    free(pszString);

    return str;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  WinCerts::Init(env, exports);
  return exports;
}

NODE_API_MODULE(WinCerts, Init)
