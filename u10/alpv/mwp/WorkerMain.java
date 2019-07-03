package alpv.mwp;

import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;

public class WorkerMain extends UnicastRemoteObject implents Worker, Runnable {

    private Master master;
    private boolean running = true;

    public WorkerMain(String host, int port) {
        System.out.println("Worker - connect to server: " + host " port: " + port);
        Registry registry = LocateRegistry.getRegistry(host, port);
        master = (Master) (registry.lookup("mwp"));
        master.registerWorker(this);
    }

    public void run() {
        System.out.println("running");
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        while (running) {
            try {
                String line = br.readLine();
                if (line != null && line.equals("quit")) {
                    master.unregisterWorker(this);
                    running = false;
                    br.close();
                }
            } catch (IOException e)
        }
    }

    public <Argument, Result> void start(Task<Argument, Result> t, Pool<Argument> argpool, Pool<Result> respool) throws RemoteException {
        Thread workerThread = new Thread(new WorkerThread<Argument, Result>(t,argpool,respool));
        workerThread.start();
    }

    class WorkerThread<Argument, Result> implents Runnable {
        Task<Argument, Result> task;
        Pool<Argument> argpool;
        Pool<Result> respool;

        public WorkerThread(Task<Argument, Result> inTask, Pool<Argument> inArgpool, Pool<Result> inRespool) {
            task = inTask;
            argpool = inArgpool;
            respool = inRespool;
        }

        public void run() {
            while(running && argpool.size() != -1){
                Argument argument = argpool.get();
                if(argument != null) {
                    Result result = task.exec(argument);
                    if(result != null) {
                        respool.put(result);
                    }
                }
            }
        }
    }
}