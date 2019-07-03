package alpv.mwp;

import java.rmi.RemoteException;

import alpv.mwp.Job;
import alpv.mwp.Pool;
import alpv.mwp.Task;
import java.util;

public class JobMath implements Job<Integer, Integer, Integer> {

	private static final long serialVersionUID = 267504255130640656L;
	private Integer[] _numbers;
	protected SquareTask _task;
	protected Integer _argument;
	protected SquareRemoteFuture _remoteFuture;
	public JobMath(Integer[] numbers) throws RemoteException {
		_task = new SquareTask();
		_numbers = numbers;
	}

	@Override
	public void split(Pool<Integer> argPool, int workerCount) {
		for (Integer number : _numbers) {
			try {
				argPool.put(number);
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
	}
	
	public SquareRemoteFuture getFuture() {
		if(_remoteFuture == null) {
			try {
				_remoteFuture = new SquareRemoteFuture();
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		}
		return _remoteFuture;
	}

	public Task<Integer, Integer> getTask() {
		return _task;
	}

	@Override
	public void merge(Pool<Integer> resPool) {
		Integer result = 0 ;
		try {
			Integer i;
			while ((i = resPool.get()) != null) {
				result += i
			}
			getFuture().set(result);
		} catch (RemoteException e) {
			// ignore
		}
		
	}
}
