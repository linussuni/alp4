package alpv.mwp;

import alpv.mwp.Task;
import java.lang.*;

public class TaskMath implements Task<Integer, Integer> {

	private static final long serialVersionUID = 1L;

	@Override
	public Integer exec(Integer number) {
		return ((int) Math.pow(number,2));
	}
}
