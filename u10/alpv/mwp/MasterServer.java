package alpv.mwp;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.List;

public class MasterServer extends UnicastRemoteObject implements Master, Server {

	static final String NAME = "mwp";
	final Registry registry;
	final List<Worker> workers;

	public MasterServer(int port) throws RemoteException {
		System.out.println("Server: init");
		registry = LocateRegistry.createRegistry(port);
		registry.rebind(NAME, this);

		workers = new ArrayList<Worker>();
		try {
			String address = (InetAddress.getLocalHost()).toString();
			System.out.println("Server running @ " + address + ":" + port);
		} catch (UnknownHostException e) {
			System.err.println("Can't determine adress.");
		}
		run();
	}

	private void run() {
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		while (true) {
			try {
				String line = br.readLine();
				if (line != null) {
					if (line.equals("q")) {
						System.out.println("bye");
						break;
					} else if (line.equals("s")) {
						System.out.println("I have " + workers.size()
								+ " workers.");
					}
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		try {
			br.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.exit(0);
	}

	@Override
	public void registerWorker(Worker w) throws RemoteException {
		System.out.println("Server: Worker registert");
		workers.add(w);
		System.out.println("Server: Workers " + workers.size());
	}

	@Override
	public void unregisterWorker(Worker w) throws RemoteException {
		System.out.println("Server: Worker unregistert");
		workers.remove(w);
	}

	@Override
	public <Argument, Result, ReturnObject> RemoteFuture<ReturnObject> doJob(
			final Job<Argument, Result, ReturnObject> job)
			throws RemoteException {
		final PoolMain<Argument> argumentPool = new PoolMain<Argument>(
				workers.size());
		job.split(argumentPool, workers.size());
		final PoolMain<Result> resultPool = new PoolMain<Result>();
		System.out.println("Server: Start job. Workers: " + workers.size());
		for (Worker w : workers) {
			w.start(job.getTask(), argumentPool, resultPool);
		}
		System.out.println("All started.");

		Thread mergeObserver = new Thread(new Runnable() {
			public void run() {
				try {
					while (argumentPool.size() != -1) {
						try {
							Thread.sleep(100);
						} catch (InterruptedException e) {
						}
					}
					System.out.println("Server: Merge");
					job.merge(resultPool);
				} catch (RemoteException e) {
					e.printStackTrace();
				}
			}
		});
		mergeObserver.start();

		return job.getFuture();
	}
}
