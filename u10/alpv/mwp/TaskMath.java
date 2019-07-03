package alpv.mwp;

import alpv.mwp.Task;

public class TaskMath implements Task<Integer, Integer> {

	private static final long serialVersionUID = 1L;

	@Override
	public Integer exec(Integer number) {
		return new Integer(math.pow(number,2));
	}
}
