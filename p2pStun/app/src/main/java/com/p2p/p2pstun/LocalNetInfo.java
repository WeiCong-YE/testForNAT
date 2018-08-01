package com.p2p.p2pstun;

public class LocalNetInfo {

//    public native void setStunServer(String serverIp, String serverPort);
    public static final byte NAT_TYPE_UNKNOW = 0;//未知的NAT类型， 可能未检测过
    public static final byte NAT_TYPE_SYMMETRIC = 1;//对称型NAT， 两端都是对称型NAT时，不能P2P
    public static final byte NAT_TYPE_NOT_SYMMETRIC = 2;//非对称型NAT， 有一端是对称型NAT则可以实现P2P的UDP打洞
    public static final byte NAT_TYPE_NOT_NAT = 3;//非NAT网络，目前一般只具有因特网IP的设备
    private byte mNetType = NAT_TYPE_UNKNOW; //本地网络类型
    private String mLocalOutIp; //本地对外IP
    private String mLocalOutPort; //本地对外端口

    /**
     *
     * @param serverIp
     * @param serverPort
     */
    public native void detectionOfLocalNet(String serverIp, String serverPort);
}
