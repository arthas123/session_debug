package tvos.tv;

import java.util.ArrayList;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.ServiceManager;
import tvos.tv.TUtils;
import com.tcl.tosapi.atv.TvPlayerApi;
import com.tcl.tvmanager.TDtvPvrManager;
import com.tcl.tvmanager.TTvChannelManager;
import com.tcl.tvmanager.TDtvTeletextManager;
import com.tcl.tvmanager.TTvCecManager;
import com.tcl.tvmanager.TTvCiManager;
import com.tcl.tvmanager.TTvCommonManager;
import com.tcl.tvmanager.TTvEpgManager;
import com.tcl.tvmanager.TTvPictureManager;
import com.tcl.tvmanager.TTvScanManager;
import com.tcl.tvmanager.ITclTvService;
import com.tcl.tvmanager.TTvCommonManager;
import java.util.HashMap;
import java.lang.Integer;
import com.tcl.tosapi.atv.TvVideoApi;
import tvos.tv.common.ITypedEventListener;
import tvos.tv.common.ITEventRegister;
import com.tcl.tosapi.listener.EventNotify;
import tvos.tv.channel.TChannelManager;

public class TSessionManager {
    private static final String TAG = "TSessionManager";

    private static Context mContext;
    private static TSessionManager sInstance = null;

    private HashMap<Integer,TClientSession> mClientSessionIDMap = new HashMap<Integer,TClientSession>();
	private TChannelManager mChannelManager;
	private TvVideoApi mTvVideoApi = null;
	
	private ITEventRegister mTEventRegister;
	
	private int mListenerId = 0;
    private SessionEventListener mSessionEventListener;

    private TSessionManager(Context context) {
        mContext = context;
        mChannelManager = TManager.getInstance(context).getChannelManager();
        mTvVideoApi = TvVideoApi.getInstance();
        mSessionEventListener = new SessionEventListener();
		mTEventRegister = TManager.getInstance(null).getEventRegister();
		if(mTEventRegister != null){
			try{
				mListenerId = mTEventRegister.registerEventListener(EventNotify.EVENT_CLIENT_SESSION,mSessionEventListener);
			}catch(Exception e){
			}
		}
    }

    public static TSessionManager getInstance(Context context) {
        synchronized(TSessionManager.class) {
            if (sInstance == null) {
                sInstance = new TSessionManager(context);
            }
        }

        return sInstance;
    }

    public class SessionEventListener extends ITypedEventListener.Stub {

        @Override
		public void onEvent(int eventType,int param0,int param1,int param2,int param3){
			Message message = Message.obtain();
			if(eventType == EventNotify.EVENT_CLIENT_SESSION){
				message.what = param0;//session id
				message.arg1 = param1;// 1- requested ,0 - released
				TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_CLIENT_SESSION,message);
			}
		}
    }
	public int createClient(int source_type) {
		//TClientSession ret = null;
		TUtils.logd(TAG, "Begin to creat Client........");
		int clientId = mTvVideoApi.createSession(source_type);
		
		/*
		if(clientId > 0){
			ret = new TClientSession(source_type,clientId);
			mClientSessionIDMap.put(Integer.valueOf(clientId),ret);
		}
		*/
		TUtils.logd(TAG, "Session Client created!!!id = "+clientId);
		return clientId;
	}

	public int destoryClient(int session_id) {
		int ret = -1;
		TUtils.logd(TAG, "Destroy Client........");
		ret = mTvVideoApi.destorySession(session_id);
		//mClientSessionIDMap.remove(Integer.valueOf(client.getSessiontId()));

		TUtils.logd(TAG, "Destroy Client success!");
		return ret;
	}

	public int requestSession(int session_id){
		TUtils.logd(TAG, "Request Session........");
		mTvVideoApi.requestSession(session_id);
		TUtils.logd(TAG, "Session Requested!!!!");
		return 0;
	}

	public void releaseSession(int session_id){
		TUtils.logd(TAG, "Release Session........");
		mTvVideoApi.releaseSession(session_id);
		TUtils.logd(TAG, "Session Released!!!!");
	}
	
	@Override
	protected void finalize() throws Throwable{
		if(mTEventRegister != null){
			try{
				if(mListenerId > 0){
					mTEventRegister.unregisterListener(mListenerId);
				}
			}catch(Exception e){
			}
		}
		super.finalize();
	}

}
