
public class ComJni
{
    public static native void callJni(String reqStr, int len);
    public static void callback(String respStr, int len)
    {
    	System.out.println("java method str:"+respStr);
    	System.out.println("java method len:"+len);
    }
}
