package com.p2p.p2pstun;

import android.os.Handler;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initEnv();
    }

    /**
     * 获取本地外网IP与端口对 Button
     * 免费stun服务器地址：
     * ① stun.freeswitch.org （64.235.150.11）  端口：3478
     * ② stun.xten.com （216.93.246.18）
     * ③ stun.ekiga.net （217.10.68.152）
     * @param view
     */
    public void GetIpAndPortButton(View view){
        switch (view.getId()){
            case R.id.getIpAndPort:
                break;
            default:
                Toast.makeText(this,
                        "不是按钮getIpAndPortButton",
                        Toast.LENGTH_SHORT).show();
        }

        EditText etServerIp = findViewById(R.id.serverIp);
        EditText etServerPort = findViewById(R.id.serverPort);//"64.235.150.11", NULL);//

        String serverIp = etServerIp.getText().toString();
        String serverPort = etServerPort.getText().toString();
        if (serverPort != null && serverPort.length() < 3){//端口号一般多于2位数
            serverPort = null;
        }

        String gettedIpAndPort = getLocatIpAndPort(serverIp, serverPort);
        if (gettedIpAndPort != null){
            EditText gettedLocalIpAndPort = findViewById(R.id.gettedLocalIpAndPort);
            gettedLocalIpAndPort.setText(gettedIpAndPort);

            String[] ipAndPort =  gettedIpAndPort.split(":");
            EditText oppositeIp = findViewById(R.id.oppositeIp);
            oppositeIp.setText(ipAndPort[0]);

            EditText oppositePort = findViewById(R.id.oppositePort);
            oppositePort.setText(ipAndPort[1]);
        } else {
            Toast.makeText(this,
                    "获取本地外网IP与端口对失败",
                    Toast.LENGTH_LONG).show();
        }
    }

    boolean mbSetOppositeNet = false;
    /**
     * 发送数据到对端， 点击前需要先设置对端IP和端口号
     * @param view
     */
    public void sendData(View view){
        if (mbSetOppositeNet) {
            EditText sendTV = findViewById(R.id.sendText);
            String sendData = sendTV.getText().toString();
            if (sendData.length() > 0) {
                int ret = sendData(sendData);
                if (ret != sendData.length()) {
                    Toast.makeText(this,
                            "要发送长度=" + sendData.length() + ", 实际发送长度=" + ret,
                            Toast.LENGTH_SHORT);
                }
            } else {
                Toast.makeText(this, "请输入要发送的数据", Toast.LENGTH_SHORT);
            }
        }
    }

    public void setOppositeNet(View view){
        EditText oppositeIpTV = findViewById(R.id.oppositeIp);
        EditText oppositePortTV = findViewById(R.id.oppositePort);
        String oppositeIp = oppositeIpTV.getText().toString();
        String oppositePort = oppositePortTV.getText().toString();

        if (oppositeIp.length() > 0 &&
                oppositePort.length() > 0){
            if (0 == setOppositeNet(oppositeIp, oppositePort)){
                mbSetOppositeNet = true;
            } else {
                Toast.makeText(this, "对端IP或端口错误！", Toast.LENGTH_LONG);
            }
        }
    }

    String mRecvData;
    Handler handler = new Handler();
    Runnable update_thread = new Runnable()
    {
        public void run()
        {
            EditText recvTV = findViewById(R.id.recevText);
            recvTV.setText(mRecvData);
        }
    };

    public void recvData(String data){
//        EditText recvTV = findViewById(R.id.recevText);
//        recvTV.setText(data);
        mRecvData = data;
        Log.w("MainActivity", "recvData: " + mRecvData);
        handler.post(update_thread);
    }



    /**
     * 初始化一些JVM环境
     */
    public native void initEnv();

    /**
     * 从指定服务器获取本地外网IP与端口对
     * @param serverIp  stun 服务器
     * @param servePort strun服务器端口号，默认(null时)为3478
     * @return 成功则返回IP与端口对，以xx.xx.xx.xx:port形式返回，失败则返回null
     */
     //TODO：此接口需要改为异步调用方式
    public native String getLocatIpAndPort(String serverIp, String servePort);

    /**
     * 设置P2P中对端的IP和端口号
     * @param peerIp 对端IP
     * @param peerPort 对端端口号
     * @return 0：设置成功； 其他值：设置失败
     */
    public native int setOppositeNet(String peerIp, String peerPort);

    /**
     * 发送数据到P2P的对端
     * @param data 要发送的数据
     * @return 发送的数据长度
     */
    public native int sendData(String data);
}
