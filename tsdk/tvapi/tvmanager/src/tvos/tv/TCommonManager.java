package tvos.tv;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import tvos.tv.TUtils;
import java.lang.Integer;

import com.tcl.tvmanager.IAtvPlayerEventCallback;
import com.tcl.tvmanager.IInputSourceSwitchCallback;
import com.tcl.tvmanager.ITVSignalCallBack;
import com.tcl.tvmanager.listener.TVSourceListener;
import com.tcl.tvmanager.vo.AudioLangInfo;
import com.tcl.tvmanager.vo.EnTCLAudioChannel;
import com.tcl.tvmanager.vo.EnTCLBridgeTiming;
import com.tcl.tvmanager.vo.EnTCLAvVideoInput;
import com.tcl.tvmanager.vo.EnTCLDolbyAudioInfo;
import com.tcl.tvmanager.vo.EnTCLEdidVersion;
import com.tcl.tvmanager.vo.EnTCLLanguage;
import com.tcl.tvmanager.vo.EnTCLMemberServiceType;
import com.tcl.tvmanager.vo.EnTCLScreenMode;
import com.tcl.tvmanager.vo.EnTCLInputSource;
import com.tcl.tvmanager.vo.EnTosSourceBootInput;
import com.tcl.tvmanager.vo.EnTCLInputSourceSwitchResult;
import com.tcl.tvmanager.vo.EnTCLSignalStatus;
import com.tcl.tvmanager.vo.EnTCLSourceName;
import com.tcl.tvmanager.vo.VideoInfo;
import com.tcl.tvmanager.vo.EnTCLProjectType;
import com.tcl.tvmanager.vo.EnTCLWindow;
import com.tcl.tvmanager.vo.EnTCLCallBackTvStatusMsg;
import com.tcl.tvmanager.vo.EnTCLCallBackSetSourceMsg;
import com.tcl.tvmanager.aidl.ITvPlayerProxy;
import com.tcl.tvmanager.aidl.ITvChannelProxy;
import com.tcl.tvmanager.vo.EnTCLLocation;
import com.tcl.tvmanager.ITOTUpdateCallBack;
import com.tcl.tvmanager.IAudioLanguageListener;
import com.tcl.tvmanager.ITclTvService;
import com.tcl.tvservice.proxy.TvPlayerProxy;
import android.view.SurfaceHolder;
import android.view.Surface;
import tvos.tv.TUtils;
import com.tcl.tosapi.atv.TvVideoApi;
import com.tcl.tosapi.atv.TvPlayerApi;
import java.util.ArrayList;
import tvos.tv.TManager;
import com.tcl.utility.property.PropertyImpl;
import com.tcl.tosapi.atv.TvVideoApi;
import tvos.tv.event.ITypedEventListener;
import tvos.tv.event.ITEventRegister;
import tvos.tv.TManager;
import com.tcl.tosapi.listener.EventNotify;

public class TCommonManager {
    private static final String TAG = "TCommonManager";
    private Context mContext;
    private ITclTvService mService;

    private static TCommonManager sInstance = null;

    private TVSignalCallBack mTVSignalCallBack;
    private TVSourceListener mTVSourceListener;
    private TOTUpdateCallBack mTOTUpdateCallBack;
    private InputSourceSwitchCallback mInputSourceSwitchCallback;
    private AtvPlayerEventCallback mAtvPlayerEventCallback;

    private TvPlayerProxy tvPlayerProxy;
    private TvPlayerApi tvPlayerApi;
    private ITvChannelProxy mIChannelProxy;
    private AudioLanguageListener mAudioLangListener;
	private PropertyImpl mProperty;
	private TvVideoApi mVideoApi;
	private ITEventRegister mTEventRegister;
	private int mListenerId0 = 0;
	private int mListenerId1 = 0;
	private int mListenerId2 = 0;
	
    private TCommonManager(Context context) throws Exception {
        mService = TManager.getInstance(context).getTvService();
        mContext = context;
        mProperty = PropertyImpl.getInstance(context);
        try {
            mIChannelProxy = mService.getTvChannelProxy();
            tvPlayerProxy = TvPlayerProxy.getInstance(context);
        } catch (RemoteException e) {
            e.printStackTrace();
            TUtils.logd(TAG, "getTvPlayerProxy error!!!");
        }

        //initInputSourceSwitchCallback();
        //initAtvPlayerEventCallback();
        initAudioLangListener();
        if(mTOTUpdateCallBack == null) {
            try {
                mTOTUpdateCallBack = new TOTUpdateCallBack();
                mIChannelProxy.registerTOTUpdateListener(mTOTUpdateCallBack);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
		tvPlayerApi = TvPlayerApi.getInstance();
		mVideoApi = TvVideoApi.getInstance();
        CommonEventListener dtvCiEventListener = new CommonEventListener();
		mTEventRegister = TManager.getInstance(null).getEventRegister();
		if(mTEventRegister != null){
			try{
				mListenerId0 = mTEventRegister.registerEventListener(EventNotify.INPUT_SOURCE_CHANGED_EVENT,dtvCiEventListener);
				mListenerId1 = mTEventRegister.registerEventListener(EventNotify.OUTPUT_SIGANL_STATUS_EVENT,dtvCiEventListener);
				mListenerId2 = mTEventRegister.registerEventListener(EventNotify.SOURCE_SWITCH_EVENT,dtvCiEventListener);
			}catch(Exception e){
			}
		}
    }
    /* === Common === [[ */
    /**
     * get current Input Source
     * @return source mode  @see EnTCLInputSource
     */
    // TODO aidl中修改为getCurrentInputSource
    public EnTCLInputSource getCurrentInputSource() {
        EnTCLInputSource source = EnTCLInputSource.EN_TCL_NONE;
        try {
        	int iSource = mVideoApi.getCurrentInputSource();
        	if(iSource >= 0 && iSource <= EnTCLInputSource.EN_TCL_MAX.ordinal()){
				source = EnTCLInputSource.values()[iSource];
        	}
        } catch (Exception e) {
            e.printStackTrace();
        }

        TUtils.logd(TAG,"getCurrentInputSource : " + source);
        return source;
    }

    /**
     * get Lastest Saved Source
     * @return source mode  @see EnTCLInputSource
     */
    public EnTCLInputSource getLastestSavedSource() {
    	EnTCLInputSource ret = EnTCLInputSource.EN_TCL_ATV;
        int sourceId = 0;
        sourceId = mVideoApi.getLastestSavedSource();
        if(sourceId > -1 && sourceId < EnTCLInputSource.values().length) {
            ret = EnTCLInputSource.values()[sourceId];
        }
        TUtils.logd(TAG,"getLastestSavedSource:sourceId="+sourceId+",ret="+ret);
        return ret;
    }

    public int setInputSource(int session_id,EnTCLInputSource inputSource,boolean issave) {
		mVideoApi.setInputSourceWithSession(session_id, inputSource.ordinal(), issave);
		return 0;
    }

    public int setBootSource(EnTosSourceBootInput bootSource) {
		return mVideoApi.setBootInput(bootSource.ordinal());
    }
    /**
     * HDMI 源于 DVI，HDMI完全向下兼容DVI
     * HDMI video format, DVI
     * @return true:HDMI, false:DVI
     */
    public boolean isHdmiMode() {
    	boolean ret = tvPlayerApi.isHdmiMode();
    	TUtils.logd(TAG,"in isHdmiMode,ret="+ret);
        return ret;
    }

    public static TCommonManager getInstance(Context context) {
        if (sInstance == null) {
            synchronized(TCommonManager.class) {
                if (sInstance == null) {
                    try {
                        sInstance = new TCommonManager(context);
                    } catch(Exception e) {
                        sInstance = null;
                    }

                }
            }
        }

        return sInstance;
    }

    /**
     * is Signal Stable
     * @param window window mode @see EnTCLWindow
     * @return <code>true</code> if succeed, <code>false</code> if failed.
     */
    public boolean isSignalStable(EnTCLWindow window) {
        return tvPlayerProxy.isSignalStable(window);
    }

    /**
     * get VideoInfo
     * @return VideoInfo @see VideoInfo
     */
    public VideoInfo getVideoInfo() {
        return tvPlayerProxy.getVideoInfo();
    }

    /**
     * get Dtv AudioLangInfo
     * @return AudioLangInfo @see AudioLangInfo
     */
    public AudioLangInfo getDtvAudioLangInfo() {
        try {
            return tvPlayerProxy.getDtvAudioLangInfo();
        } catch (RemoteException e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * set Dtv AudioLangInfo
     * @param lang audioIndex
     */
    public void setDtvAudioLangInfo(int lang) {
        try {
            tvPlayerProxy.setDtvAudioLangInfo(lang);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    /**
     * get Dolby AudioInfo
     * @return EnTCLDolbyAudioInfo @see EnTCLDolbyAudioInfo
     */
    public EnTCLDolbyAudioInfo getDolbyAudioInfo() {
        try {
            return tvPlayerProxy.getDolbyAudioInfo();
        } catch (RemoteException e) {
            e.printStackTrace();
            return EnTCLDolbyAudioInfo.EN_TCL_NONE;
        }
    }

    /**
     * get Dtv AudioChannel
     * @return EnTCLAudioChannel @see EnTCLAudioChannel
     */
    public EnTCLAudioChannel getDtvAudioChannel() {
        try {
            return tvPlayerProxy.getDtvAudioChannel();
        } catch (RemoteException e) {
            e.printStackTrace();
            return EnTCLAudioChannel.EN_TCL_STEREO;
        }
    }

    /**
     * set Dtv AudioChannel
     * @param channel AudioChannel mode@see EnTCLAudioChannel
     */
    public void setDtvAudioChannel(EnTCLAudioChannel channel) {
        try {
            tvPlayerProxy.setDtvAudioChannel(channel);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    /**
     * set Display
     * @param sh android.view.SurfaceHolder
     * @return <code>true</code> if succeed, <code>false</code> if failed.
     */
    public boolean setDisplay(SurfaceHolder sh) {
        if (sh != null) {
            return tvPlayerProxy.setSurface(sh.getSurface());
        } else {
            return false;
        }
    }

    /**
     * get screen saver mode
     * @return EnTCLScreenMode @see EnTCLScreenMode
     */
    public EnTCLScreenMode getScreenModeStatus() {
        EnTCLScreenMode ret = EnTCLScreenMode.EN_TCL_DTV_SS_INVALID_SERVICE;
    	int iStatus = mVideoApi.getScreenModeStatus();
    	if(iStatus >= 0 && iStatus <= EnTCLScreenMode.EN_TCL_DTV_SS_MAX.ordinal()){
            ret = EnTCLScreenMode.values()[iStatus];
    	}
        return ret;
    }

    /**
     * 待机时释放DTV中间件资源
     * @return <code>true</code> if succeed, <code>false</code> if failed.
     */
    private boolean middleWareFinalize() {
        return tvPlayerProxy.middleWareFinalize();
    }

    /**
     * 检测指定的信源类型的信号线是否接入�
     * @param inputSource 指定的信源类型�
     * @return <code>true</code> if succeed, <code>false</code> if failed.
     */
    public boolean isInserted(EnTCLInputSource inputSource) {
        return tvPlayerProxy.isInserted(inputSource);
    }

    /**
     * get ProjectType
     * @return EnTCLProjectType @see EnTCLProjectType
     */
    public EnTCLProjectType getProjectType() {
        return tvPlayerProxy.getProjectType();
    }

    /**
     * get signal status of EnTCLWindow
     * @param window  EnTCLWindow @see EnTCLWindow
     * @return EnTCLSignalStatus @see EnTCLSignalStatus
     */
    public EnTCLSignalStatus getSignalStatus(EnTCLWindow window) {
        return tvPlayerProxy.getSignalStatus(window);
    }

    private class TOTUpdateCallBack extends ITOTUpdateCallBack.Stub {
        @Override
        public void OnToTUpdate() {
            TUtils.logd(TAG,"rengb******TOTUpdateCallBack,TCommonManager");
            Message msg = Message.obtain();
            msg.what = EnTCLCallBackTvStatusMsg.EN_TCL_TOT_UPDATE.ordinal();
            TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_TOT_UPDATE_EVENT,msg);
        }
    }

    private class  TVSignalCallBack extends ITVSignalCallBack.Stub {
        @Override
        public void onOutputSignalStatus(int windowId, boolean signalStatus) throws RemoteException
        {
            Bundle b = new Bundle();
            Message msg = Message.obtain();
            if ( signalStatus == true ) {
                msg.what = EnTCLCallBackTvStatusMsg.EN_TCL_HAS_SIGNAL.ordinal();
            } else {
                msg.what = EnTCLCallBackTvStatusMsg.EN_TCL_NO_SIGNAL.ordinal();


            }
            b.putInt("windowId",windowId);
            msg.setData(b);
            TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_STATUS_UPDATE,msg);

            TUtils.logd(TAG,"onOutputSignalStatus-->"+signalStatus);
        }
    }

    /*private void initInputSourceSwitchCallback() {
        TUtils.logd(TAG,"initInputSourceSwitchCallback...");
        if (mInputSourceSwitchCallback == null) {
            mInputSourceSwitchCallback = new InputSourceSwitchCallback();
            try {
                mIChannelProxy.registerInputSourceSwitchCallback("tcl", mInputSourceSwitchCallback);
            } catch (RemoteException e) {
                TUtils.logd(TAG,"registerInputSourceSwitchCallback remote exception...");
            }
        }
    }*/

    private class InputSourceSwitchCallback extends IInputSourceSwitchCallback.Stub {

        @Override
        public void onSwitch(/*EnTCLInputSource*/int targetSource, /*EnTCLInputSourceSwitchResult*/int result)
        throws RemoteException {
            TUtils.logd(TAG,"TvService call IInputSourceSwitchCallback.Stub, onSwitch...");
            int what = -1;
            what = EnTCLCallBackSetSourceMsg.EN_TCL_SET_SOURCE_START.ordinal();
            if (result == EnTCLInputSourceSwitchResult.EN_TCL_END_SUCESSED.ordinal()) {
                what = EnTCLCallBackSetSourceMsg.EN_TCL_SET_SOURCE_END_SUCCEED.ordinal();
            } else if (result == EnTCLInputSourceSwitchResult.EN_TCL_END_FAILED.ordinal()) {
                what = EnTCLCallBackSetSourceMsg.EN_TCL_SET_SOURCE_END_FAILED.ordinal();
            }

            Message sourceSwitchMessage = Message.obtain();
            sourceSwitchMessage.what = what;
            TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_SET_SOURCE,sourceSwitchMessage);

        }
    }

    private void initAtvPlayerEventCallback() {
        TUtils.logd(TAG,"initAtvPlayerEventCallback...");
        if (mAtvPlayerEventCallback == null) {
            mAtvPlayerEventCallback = new AtvPlayerEventCallback();
            try {
                mIChannelProxy.registerAtvPlayerEventCallback("tcl", mAtvPlayerEventCallback);
            } catch (RemoteException e) {
                TUtils.logd(TAG,"registerAtvPlayerEventCallback remote exception...");
            }
        }
    }

    private class CommonEventListener extends ITypedEventListener.Stub {

        @Override
		public void onEvent(int eventType,int parma0,int parma1,int parma2,int parma3)
        {
			TUtils.logd(TAG,"in CommonEventListener onEvent("+eventType+","+parma0+","+parma1+","+parma2+","+parma3+")");
			int what;
			Message message;
            if(EventNotify.INPUT_SOURCE_CHANGED_EVENT == eventType){
				if (parma1 == 1) {
					what = EnTCLCallBackTvStatusMsg.EN_TCL_INPUT_CHANGE_HAS_SIGANL.ordinal();
				} else {
					what = EnTCLCallBackTvStatusMsg.EN_TCL_INPUT_CHANGE_NO_SIGANL.ordinal();
				}
				message = Message.obtain();
				message.what = what;
				message.obj = EnTCLInputSource.values()[parma0];
				TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_STATUS_UPDATE,message);
            }
            else if(EventNotify.OUTPUT_SIGANL_STATUS_EVENT == eventType){
				EnTCLSignalStatus status = EnTCLSignalStatus.values()[parma1];
				switch (status) {
				case EN_TCL_NO_SIGNAL:
					what = EnTCLCallBackTvStatusMsg.EN_TCL_NO_SIGNAL.ordinal();
					break;
				case EN_TCL_STABLE:
					what = EnTCLCallBackTvStatusMsg.EN_TCL_HAS_SIGNAL.ordinal();
					break;
				case EN_TCL_UNSUPPORTED:
					what = EnTCLCallBackTvStatusMsg.EN_TCL_UNSUPPORT_SIGNAL.ordinal();
					break;
				case EN_TCL_UNSTABLE:
					what = EnTCLCallBackTvStatusMsg.EN_TCL_UNSTABLE_SIGNAL.ordinal();
					break;
				default:
					return;
				}
				message = Message.obtain();
				message.what = what;
				
				TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_STATUS_UPDATE,message);
            }
            else if(EventNotify.SOURCE_SWITCH_EVENT == eventType){
				int result = parma1;
				what = EnTCLCallBackSetSourceMsg.EN_TCL_SET_SOURCE_START.ordinal();
				if (result == EnTCLInputSourceSwitchResult.EN_TCL_END_SUCESSED.ordinal()) {
					what = EnTCLCallBackSetSourceMsg.EN_TCL_SET_SOURCE_END_SUCCEED.ordinal();
				} else if (result == EnTCLInputSourceSwitchResult.EN_TCL_END_FAILED.ordinal()) {
					what = EnTCLCallBackSetSourceMsg.EN_TCL_SET_SOURCE_END_FAILED.ordinal();
				}
				
				Message sourceSwitchMessage = Message.obtain();
				sourceSwitchMessage.what = what;
				TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_SET_SOURCE,sourceSwitchMessage);
				
				message = Message.obtain();
				message.what = TUtils.MESSAGE_TYPE_SOURCE_CHANGED;
				TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_SOURCE_CHANGED,message);
            }
        }

    }

    private class AtvPlayerEventCallback extends IAtvPlayerEventCallback.Stub {
        @Override
        public void onInputSignalChanged(/*EnTCLInputSource*/int source, boolean hasSignal) {
            TUtils.logd(TAG,"TvService call IAtvPlayerEventCallback.Stub, onInputSignalChanged...source:" +
                        source + ",hasSignal:" + hasSignal);
            int what;
            if (hasSignal) {
                what = EnTCLCallBackTvStatusMsg.EN_TCL_INPUT_CHANGE_HAS_SIGANL.ordinal();
            } else {
                what = EnTCLCallBackTvStatusMsg.EN_TCL_INPUT_CHANGE_NO_SIGANL.ordinal();
            }
            Message sourceInChangedMessage = Message.obtain();
            sourceInChangedMessage.what = what;
            sourceInChangedMessage.obj = EnTCLInputSource.values()[source];
            TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_STATUS_UPDATE,sourceInChangedMessage);
        }

        @Override
        public void onSignalStatusChanged(/*EnTCLInputSource*/int currentSource, /*EnTCLSignalStatus*/int currentSignalStatus) {
            TUtils.logd(TAG,"TvService call IAtvPlayerEventCallback.Stub, onSignalStatusChanged...currentSource:" +
                        currentSource + ", currentSignalStatus:" + currentSignalStatus);
            int what = -1;
            EnTCLSignalStatus status = EnTCLSignalStatus.values()[currentSignalStatus];
            switch (status) {
            case EN_TCL_NO_SIGNAL:
                what = EnTCLCallBackTvStatusMsg.EN_TCL_NO_SIGNAL.ordinal();
                break;
            case EN_TCL_STABLE:
                what = EnTCLCallBackTvStatusMsg.EN_TCL_HAS_SIGNAL.ordinal();
                break;
            case EN_TCL_UNSUPPORTED:
                what = EnTCLCallBackTvStatusMsg.EN_TCL_UNSUPPORT_SIGNAL.ordinal();
                break;
            case EN_TCL_UNSTABLE:
                what = EnTCLCallBackTvStatusMsg.EN_TCL_UNSTABLE_SIGNAL.ordinal();
                break;
            default:
                return;
            }
            Message signalLockMessage = Message.obtain();
            signalLockMessage.what = what;

            TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_STATUS_UPDATE,signalLockMessage);
        }
        
    }
    /**
     * set ChildrenMode
     * @param enabled : boolean
     */
    public void setChildrenMode(boolean enabled) {
        // TODO
        try {
            tvPlayerProxy.setChildrenMode(enabled);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }
    /**
     * get ChildrenMode
     * @param enabled
     */
    public boolean getChildrenMode() {
        try {
            return tvPlayerProxy.getChildrenMode();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return false;
    }
    /**
     * 获取是否支持儿童模式功能
     * @param enabled
     */
    public boolean getSupportChildrenMode() {
        try {
            return tvPlayerProxy.getSupportChildrenMode();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return false;
    }
    /**
     * 儿童模式是否使能信号源
     * @param enabled
     */
    public void enableSignalForChildrenMode(boolean enabled) {
        try {
            tvPlayerProxy.enableSignalForChildrenMode(enabled);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    /**
     * get location, the point to get value, EN_HOME, EN_SHOP, EN_SHOP_WITH_DEMO
     * @return EnTCLLocation @see EnTCLLocation
     */
    public EnTCLLocation getLocation() {
        EnTCLLocation ret = EnTCLLocation.EN_HOME;
        String saved = mProperty.get("tvmanager.EnTCLLocation","0");
        if(saved != null){
        	int temp = Integer.parseInt(saved);
        	if(temp >= 0 && temp< EnTCLLocation.EN_MAX.ordinal()){
				ret = EnTCLLocation.values()[temp];
        	}
        }
        return ret;
    }

    /**
     * set location, the point to set value, EN_HOME, EN_SHOP, EN_SHOP_WITH_DEMO
     * @param location EnTCLLocation @see EnTCLLocation
     */
    public void setLocation(EnTCLLocation location) {
        mProperty.set("tvmanager.EnTCLLocation",Integer.toString(location.ordinal()));
    }

    /**
     * set inputsource name
     * @param source EnTCLInputSource @see EnTCLInputSource
     * @param name EnTCLSourceName @see  EnTCLSourceName
     */
    public void setInputSourceName(EnTCLInputSource source, EnTCLSourceName name) {
        mProperty.set("EnTCLInputSource."+source.ordinal(),Integer.toString(name.ordinal()));
    }

    /**
     * get inputsource name
     * @param source EnTCLInputSource @see EnTCLInputSource
     * @return EnTCLSourceName @see  EnTCLSourceName
     */
    public EnTCLSourceName getInputSourceName(EnTCLInputSource source) {
        EnTCLSourceName ret = EnTCLSourceName.EN_TCL_SOURCE_NAME_OTHER;
        String saved = mProperty.get("EnTCLInputSource."+source.ordinal(),"0");
        if(saved != null){
        	int temp = Integer.parseInt(saved);
        	if(temp >= 0 && temp< EnTCLSourceName.EN_TCL_SOURCE_NAME_MAX.ordinal()){
				ret = EnTCLSourceName.values()[temp];
        	}
        }
        return ret;
    }

    /**
     * set AV videoInput
     * @param input EnTCLAvVideoInput @see EnTCLAvVideoInput
     */
    public void setAvVideoInput(EnTCLAvVideoInput input) {
        try {
            if(tvPlayerProxy !=null) {
                tvPlayerProxy.setAvVideoInput(input);
            }
        } catch (Exception e) {
        }
    }

    /**
     * @return EnTCLAvVideoInput @see EnTCLAvVideoInput
     */
    public EnTCLAvVideoInput getAvVideoInput() {
        EnTCLAvVideoInput mRet = EnTCLAvVideoInput.EN_TCL_AV_AUTO;
        try {
            if(tvPlayerProxy !=null) {
                mRet = tvPlayerProxy.getAvVideoInput();
            }
        } catch (Exception e) {
        }
        return mRet;
    }

    /**
     * set first AudioLanguage
     * @param lang EnTCLLanguage @see EnTCLLanguage
     */
    public void setFirstAudioLanguage(EnTCLLanguage lang) {
        try {
            if(tvPlayerProxy !=null) {
                tvPlayerProxy.setFirstAudioLanguage(lang);
            }
        } catch (Exception e) {
        }
    }

    /**
     * get first AudioLanguage
     * @return EnTCLLanguage @see EnTCLLanguage
     */
    public EnTCLLanguage getFirstAudioLanguage() {
        EnTCLLanguage mRet = EnTCLLanguage.EN_TCL_LANGUAGE_CHINESE;
        try {
            if(tvPlayerProxy !=null) {
                mRet = tvPlayerProxy.getFirstAudioLanguage();
            }
        } catch (Exception e) {
        }
        return mRet;
    }

    /**
     * set second AudioLanguage
     * @param lang EnTCLLanguage @see EnTCLLanguage
     */
    public void setSecondAudioLanguage(EnTCLLanguage lang) {
        try {
            if(tvPlayerProxy !=null) {
                tvPlayerProxy.setSecondAudioLanguage(lang);
            }
        } catch (Exception e) {
        }
    }

    /**
     * get second AudioLanguage
     * @return EnTCLLanguage @see EnTCLLanguage
     */
    public EnTCLLanguage getSecondAudioLanguage() {
        EnTCLLanguage mRet = EnTCLLanguage.EN_TCL_LANGUAGE_ENGLISH;
        try {
            if(tvPlayerProxy !=null) {
                mRet = tvPlayerProxy.getSecondAudioLanguage();
            }
        } catch (Exception e) {
        }
        return mRet;
    }


    /**
     * get LastServiceType
     * @return EnTCLMemberServiceType @see EnTCLMemberServiceType
     */
    public EnTCLMemberServiceType getLastServiceType() {
        EnTCLMemberServiceType mRet = EnTCLMemberServiceType.EN_TCL_ATV;
        try {
            if(tvPlayerProxy !=null) {
                mRet = tvPlayerProxy.getLastServiceType();
            }
        } catch (Exception e) {
        }
        return mRet;
    }

    /**
     * set LastServiceType
     * @param svcType EnTCLMemberServiceType @see EnTCLMemberServiceType
     */
    public void setLastServiceType(EnTCLMemberServiceType svcType) {
        try {
            if(tvPlayerProxy !=null) {
                tvPlayerProxy.setLastServiceType(svcType);
            }
        } catch (Exception e) {
        }
    }

    /**
     * 将2D,3D的信号参数发给卡端
     * @param bridgeTiming
     * @return <code>true</code> if succeed, <code>false</code> if failed.
     */
    public boolean setSignalBridgeTiming(EnTCLBridgeTiming bridgeTiming) {
        return tvPlayerApi.setSignalBridgeTiming(bridgeTiming.ordinal());
    }
    private void initAudioLangListener() {
        TUtils.logd(TAG,"initAudioLangListener...");
        if (mAudioLangListener == null) {
            mAudioLangListener = new AudioLanguageListener();
            try {
                mIChannelProxy.registerAudioLangListener("tcl", mAudioLangListener);
            } catch (RemoteException e) {
                TUtils.logd(TAG,"registerAudioLangListener remote exception...");
            }
        }
    }
    private class AudioLanguageListener extends IAudioLanguageListener.Stub {

        public void onAudioLangUpdate() {
            //TUtils.logd(TAG,"TvService call IAudioLanguageListener.Stub, OnAudioLangUpdate...");
            Message msg = Message.obtain();
            msg.what = EnTCLCallBackTvStatusMsg.EN_TCL_AUDIO_LANGUAGE_INFO_UPDATED.ordinal();
            TManager.getInstance(mContext).sendMessage(TUtils.MESSAGE_TYPE_STATUS_UPDATE,msg);
        }
    }
	
    public boolean setHdmiEdidVer(EnTCLInputSource inputSource, EnTCLEdidVersion version) {
        int ret = -1;
        if(tvPlayerApi != null) {
            ret = tvPlayerApi.setHdmiEdidVer(inputSource.ordinal(), version);
        }
        return (ret == 0) ? true : false;
    }
	@Override
	protected void finalize() throws Throwable{
		if(mTEventRegister != null){
			try{
				mTEventRegister.unregisterListener(mListenerId0);
				mTEventRegister.unregisterListener(mListenerId1);
				mTEventRegister.unregisterListener(mListenerId2);
			}catch(Exception e){
			}
		}
		super.finalize();
	}
}
