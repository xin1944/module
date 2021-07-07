
import java.util.concurrent.TimeUnit;

public class Myjava
{
    public static void main (String[] args)
    {
        System.out.println("app start");
        System.loadLibrary("myjni");
        ComJni jni = new ComJni();
        jni.callJni("test request", 15);
        System.out.println("app exit");
        while(true)
        {
        try {
			TimeUnit.SECONDS.sleep(1);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
        }
    }
}
