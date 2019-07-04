package alpv.mwp;

import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class ClientMath {

	Server clientserver;

	public ClientMath(String host, int port) throws RemoteException, NotBoundException {
		Registry registry = LocateRegistry.getRegistry(host, port);
		clientserver = (Server) (registry.lookup("mwp"));
		System.out.println("Client started.");
	}

	public void execute() throws RemoteException {
		Integer[] numbers = { 2, 3, 4 };
		Job<Integer, Integer, Integer> job = new JobMath(numbers);

		RemoteFuture<Integer> remoteFuture = clientserver.doJob(job);
		System.out.println(remoteFuture.get());
	}
}
