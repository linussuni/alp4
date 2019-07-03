import alpv.mwp;

import java.rmi.RemoteException;

public class Main {

    String defaultHost = "127.0.0.1";
    int defaultPort = 6666;
    int defaultWorkers = 4;

    public static void main(String[] args) {
        if (args.length == 0) {
            System.err.println("no.");
        } else if (args[0].equals("server")) {
            new MasterServer(defaultPort);
        } else if (args[0].equals("client")) {
            Client client = new ClientMath(defaultHost, defaultPort);
            client.execute();
        } else if (args[0].equals("worker")) {
            WorkerMain worker = new WorkerMain(defaultHost, defaultPort);
            Thread workerThread = new Thread(worker);
            workerThread.start();
        }
        
    }

}