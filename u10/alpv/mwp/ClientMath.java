package alpv.mwp;

import java.rmi.NotBoundException;
import java.rmi.RemoteException;

import alpv.mwp.Client;
import alpv.mwp.Job;
import alpv.mwp.RemoteFuture;

public class SquareClient extends Client {

	public SquareClient(String host, int port) throws RemoteException,
			NotBoundException {
		super(host, port);
	}

	public void execute() throws RemoteException {
		Integer[] numbers = { 2, 3, 4 };
		Job<Integer, Integer, Integer> job = new SquareJob(numbers);

		RemoteFuture<Integer> remoteFuture = _server.doJob(job);
		System.out.println(remoteFuture.get());
	}
}
