/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: sbinder_signal.sidl
 */
 

#define LOG_TAG     "sbinder_session"

#include <clog.h>
#include <string.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include "tos_session.h"
#include "session.h"
#include "tos_input_source.h"
#include "fpi_debug_helper.h"

SET_MODEL_ID(SITA_MODULE_SESSION);


#define SERVICE_DESCRIPTOR                      "d443efb6-398f-11e5-ab68-005056a86db3"
#define SERVICE_NAME                            "sbinder_session"

#define FIRST_CALL_TRANSACTION                  (1)
#define TRANSACTION_tos_session_creat   FIRST_CALL_TRANSACTION + 1
#define TRANSACTION_tos_session_destroy   FIRST_CALL_TRANSACTION + 2

using namespace sita_android;

#if defined(BINDER_SERVICE)

#include <stdlib.h>
#include "sbinder_session.h"

class sbinder_session_service : public BBinder  
{  
private:
    static sbinder_session_service* _instance;

    sbinder_session_service(){
        fpi_info(SERVICE_NAME"_service create\n");
    }
    
    virtual ~sbinder_session_service() {
        fpi_info(SERVICE_NAME"_service destory\n");
    }

public:  
    static int Instance() {
        if(_instance == NULL) {
            _instance = new sbinder_session_service();
            int ret = defaultServiceManager()->addService(  
                String16(SERVICE_NAME), _instance );  
            fpi_info(SERVICE_NAME"_service Instance %d\n",ret);
            return ret;  
        }

        return -1;
    }
    
    virtual status_t onTransact(uint32_t code , const Parcel& data , Parcel* reply, uint32_t flags) {
        fpi_info(SERVICE_NAME"_service - onTransact code=%d",code);

        switch(code)  {

            case TRANSACTION_tos_session_creat:
            {
		  data.enforceInterface(String16(SERVICE_NAME));  //fixed check

                //-- begin code for tos_session_create here, will auoto generated but may change yourself if need --

                //skip Exception and result first to write output paramters
                reply->writeNoException();
                //skip Exception and result first end

                //begin paramters list
                EM_SESSION_TYPE source_type = (EM_SESSION_TYPE)data.readInt32();  //EM_SESSION_TYPE as input paramter

                int _session_id_len = data.readInt32(); //read length, only 32bits length support yet
                uint32_t* session_id = NULL;

                Parcel::WritableBlob _session_id_wblob;

                if(_session_id_len > 0) {
                    reply->writeInt32(_session_id_len);
                    reply->writeBlob(_session_id_len,&_session_id_wblob);
                    session_id = (uint32_t*)_session_id_wblob.data();
                }

                // re-get all pointers from blob to fix realloc issue in Parcel
                session_id = (uint32_t*)_session_id_wblob.data();
                fpi_error _result = tos_session_create( source_type, session_id );

                reply->writeInt32(_result); //fpi_error as return value

                //-- end code for tos_session_create here --
                return NO_ERROR;
	    }break;
            
            case TRANSACTION_tos_session_destroy:
            {
		  data.enforceInterface(String16(SERVICE_NAME));  //fixed check

                //-- begin code for tos_source_set_input here, will auoto generated but may change yourself if need --

                //skip Exception and result first to write output paramters
                reply->writeNoException();
                //skip Exception and result first end

                //begin paramters list
				
                uint32_t session_id = (uint32_t)data.readInt32();  //int32_t as input paramter
		 
                // re-get all pointers from blob to fix realloc issue in Parcel
                fpi_error _result = tos_session_destory(session_id);

                reply->writeInt32(_result); //fpi_error as return value

                //-- end code for tos_source_set_input here --
                fpi_trace(SERVICE_NAME" end(code=%d)\n",code);return NO_ERROR;
	    }break;
            
            default:  
                return BBinder::onTransact(code, data, reply, flags);
        
        }

        fpi_info(SERVICE_NAME"_service end");
    }
}; 

int sbinder_session_service_add() {
    sbinder_session_service::Instance();
    return 0;
}

sbinder_session_service* sbinder_session_service::_instance = NULL;

#elif defined(BINDER_CLIENT)

class sbinder_session_client  
{  
private:
    static sbinder_session_client* _instance;
    sp<IBinder> _binder;

    sbinder_session_client()
        :_binder(NULL) {
        fpi_info(SERVICE_NAME"_client create\n");
        getService();
    }
    
    virtual ~sbinder_session_client() {
        fpi_info(SERVICE_NAME"_client destory\n");
    }

    inline bool getService(void) {
        if(_binder == NULL) {
            sp<IServiceManager> sm = defaultServiceManager();
            _binder = sm->getService(String16(SERVICE_NAME));
        }

        if(_binder == NULL) {
            fpi_warn(SERVICE_NAME"_client getFailed!\n");
        }

        return (_binder != NULL);
    }

public:  
    static sbinder_session_client* Instance() {
        if(_instance == NULL) {
            fpi_info(SERVICE_NAME"_client Instance");
            _instance = new sbinder_session_client();
        }

        return _instance;
    }

    fpi_error tos_session_create(EM_SESSION_TYPE source_type, uint32_t * session_id){
Parcel data, reply;

        /*-- add you code for tos_session_create here --*/
        fpi_error _result = FPI_ERROR_FAIL;
        if(!getService()) {
            return _result;
        }

        try {
            data.writeInterfaceToken(String16(SERVICE_NAME));//fixed check

            data.writeInt32(source_type);  //EM_SESSION_TYPE as input paramter

            //for out only, only write length ; for in, alloc and copy
            if (session_id == NULL) {
                data.writeInt32(-1);
            }
            else {
                data.writeInt32((int)(1) * sizeof(uint32_t));//write length, only support 32 bits length yet

            }

            _binder->transact(TRANSACTION_tos_session_creat,data, &reply,0);

            if(reply.readExceptionCode() == 0) {//fix check

                if (session_id != NULL) {
                    int _session_id_len = reply.readInt32(); //read length, only 32bits length support yet
                    Parcel::ReadableBlob _session_id_rblob;
                    reply.readBlob(_session_id_len,&_session_id_rblob);
                    memcpy(session_id,_session_id_rblob.data(),_session_id_len);
                }
                _result = (typeof(_result))reply.readInt32();//int as return value

            } else {
                TOSLOGW(SERVICE_NAME"_client tos_session_create error");
            }
        }catch(...) {
            TOSLOGW(SERVICE_NAME"_client tos_session_create error");
        }

        return _result;
        /*-- add you code for tos_session_create here --*/
    }

    fpi_error tos_session_destory(uint32_t session_id){
	Parcel data, reply;

	/*-- add you code for tos_session_destory here --*/
	 fpi_error _result = FPI_ERROR_FAIL;
        if(!getService()) {
            return _result;
        }

        try {
            data.writeInterfaceToken(String16(SERVICE_NAME));//fixed check

	     data.writeInt32(session_id); //int32_t as input paramter

            _binder->transact(TRANSACTION_tos_session_destroy,data, &reply,0);

            if(reply.readExceptionCode() == 0) {//fix check

                _result = (typeof(_result))reply.readInt32();//int as return value

            } else {
                fpi_warn(SERVICE_NAME"_client tos_session_destory error");
            }
        }catch(...) {
            fpi_warn(SERVICE_NAME"_client tos_session_destory error");
        }

        return _result;
        /*-- add you code for tos_session_destory here --*/
    }
};

sbinder_session_client* sbinder_session_client::_instance = NULL;

fpi_error tos_session_create(EM_SESSION_TYPE source_type, uint32_t * session_id){
    return sbinder_session_client::Instance()->tos_session_create(source_type, session_id);
}
fpi_error tos_session_destory(uint32_t session_id){
    return sbinder_session_client::Instance()->tos_session_destory(session_id);
}

#else
 #error Must define BINDER_SERVICE or BINDER_CLIENT
#endif
