package alpv.mwp;

import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayDeque;

public class PoolMain<T> extends UnicastRemoteObject implements Pool<T> {

    final ArrayDeque<T> queue;

    public PoolMain() throws RemoteException{
        queue = new ArrayDeque<T>();
    }

    // mutex to do stuff in queue
    public synchronized void put(T t) throws RemoteException {
        queue.add(t);
    }

    public synchronized T get() throws RemoteException {
        T result = queue.poll();
        return result;
    }

    public synchronized int size() throws RemoteException {
        return queue.size();
    }

}