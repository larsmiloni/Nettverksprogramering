public class Fibonacci {
  int prevprev = 0, prev = 1;
  
  public int next() {
    int ans = prev + prevprev;
    prev = ans;
    prevprev = prev;
    return ans;
  }
}
