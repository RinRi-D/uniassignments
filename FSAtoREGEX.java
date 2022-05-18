import java.io.*;
import java.lang.*;
import java.util.*;

class Edge {
    public int s, a;
    Edge(int s, int a) {
        this.s = s;
        this.a = a;
    }

    public String toString() {
        return this.s + ", " + this.a;
    }
}

class FSM {
    private ArrayList<String> states, alpha, finalState, transitions;
    private String initialState;
    private Integer initialStateID;
    private ArrayList<ArrayList<Edge>> tr;
    private ArrayList<ArrayList<Edge>> tr2way;
    private HashMap<String, Integer> stateToInt, alphaToInt;
    private HashSet<Integer> isFinal;
    private FileWriter out;
    private Integer szs, sza;
    private Boolean used[];
    private ArrayList<ArrayList<ArrayList<String>>> r;

    public void error(String msg) throws IOException {
        out.write("Error:\n" + msg);
        out.close();
        System.exit(0);
    }

    int dfs(int v) {
        int ans = 1, n = tr.get(v).size();
        used[v] = true;

        for (int i = 0; i < n; ++i) {
            int to = tr.get(v).get(i).s;
            if (!used[to])
                ans += dfs(to);
        }

        return ans;
    }

    void dfs2way(int v) {
        used[v] = true;
        int n = tr2way.get(v).size();

        for (int i = 0; i < n; ++i) {
            int to = tr2way.get(v).get(i).s;
            if (!used[to])
                dfs2way(to);
        }
    }

    FSM(String states, String alpha, String initialState, String finalState, String transitions,
        FileWriter out)
    throws IOException {
        this.out = out;
        // validate input
        if (!states.matches("states=\\[.*\\]$") || !alpha.matches("alpha=\\[.*\\]$")
            || !initialState.matches("initial=\\[.*\\]$")
            || !finalState.matches("accepting=\\[.*\\]$")
            || !transitions.matches("trans=\\[.*\\]$"))
            error("E0: Input file is malformed");

        this.states = new ArrayList<String>(Arrays.asList(states.split("states=\\[|,|\\]")));
        this.alpha = new ArrayList<String>(Arrays.asList(alpha.split("alpha=\\[|,|\\]")));
        this.finalState =
            new ArrayList<String>(Arrays.asList(finalState.split("accepting=\\[|,|\\]")));
        this.transitions =
            new ArrayList<String>(Arrays.asList(transitions.split("trans=\\[|,|\\]")));

        // initialization
        this.szs = this.states.size() - 1;
        this.sza = this.alpha.size() - 1;
        if (szs <= 0 || sza <= 0) {
            error("E0: Input file is malformed");
        }

        stateToInt = new HashMap<String, Integer>();
        alphaToInt = new HashMap<String, Integer>();
        isFinal = new HashSet<Integer>();
        tr = new ArrayList<ArrayList<Edge>>();
        tr2way = new ArrayList<ArrayList<Edge>>();
        for (int i = 0; i < szs; ++i) {
            tr.add(new ArrayList<Edge>());
            tr2way.add(new ArrayList<Edge>());
        }
        used = new Boolean[szs];

        // initialize R[k][i][j]; 0 <= k <= n; 0 <= i, j < n;
        r = new ArrayList<ArrayList<ArrayList<String>>>();
        for (int k = 0; k < szs + 1; k++) {
            r.add(new ArrayList<ArrayList<String>>());
            for (int i = 0; i < szs; ++i) {
                r.get(k).add(new ArrayList<String>());
                for (int j = 0; j < szs; ++j) {
                    r.get(k).get(i).add(new String());
                }
            }
        }

        // create a graph with nodes starting from 0
        for (int i = 1; i <= szs; ++i) this.stateToInt.put(this.states.get(i), i - 1);

        // validate transitions
        for (int i = 1; i < this.transitions.size(); ++i) {
            String[] cur = this.transitions.get(i).split(">");

            if (cur.length != 3)
                error("E0: Input file is malformed");
        }

        for (int i = 1; i < this.finalState.size(); ++i) {
            if (this.stateToInt.containsKey(this.finalState.get(i)))
                isFinal.add(this.stateToInt.get(this.finalState.get(i)));
            else
                error("E1: A state '" + this.finalState.get(i) + "' is not in the set of states");
        }

        for (int i = 1; i <= sza; ++i) alphaToInt.put(this.alpha.get(i), i - 1);

        for (int i = 1; i < this.transitions.size(); ++i) {
            String[] cur = this.transitions.get(i).split(">");

            if (cur.length != 3)
                error("E0: Input file is malformed");

            if (!this.stateToInt.containsKey(cur[0]))
                error("E1: A state '" + cur[0] + "' is not in the set of states");

            if (!this.stateToInt.containsKey(cur[2]))
                error("E1: A state '" + cur[2] + "' is not in the set of states");

            if (!this.alphaToInt.containsKey(cur[1]))
                error("E3: A transition '" + cur[1] + "' is not represented in the alphabet");

            int l = stateToInt.get(cur[0]), m = alphaToInt.get(cur[1]), r = stateToInt.get(cur[2]);
            tr.get(l).add(new Edge(r, m));
            tr2way.get(l).add(new Edge(r, m));
            tr2way.get(r).add(new Edge(l, m));
        }

        int components = 0;
        for (int i = 0; i < szs; ++i) used[i] = false;
        for (int i = 0; i < szs; ++i) {
            if (!used[i]) {
                dfs2way(i);
                components++;
            }
        }

        if (components > 1)
            error("E2: Some states are disjoint");

        for (int i = 0; i < szs; ++i) used[i] = false;

        String[] temp = initialState.split("initial=\\[|\\]");
        if (temp.length <= 1)
            error("E4: Initial state is not defined");
        this.initialState = temp[1];

        if (stateToInt.containsKey(this.initialState))
            initialStateID = stateToInt.get(this.initialState);
        else
            error("E1: A state '" + this.initialState + "' is not in the set of states");

        Boolean isDet = true, isComplete = true;
        for (int i = 0; i < szs; ++i) {
            HashSet<Integer> st = new HashSet<Integer>();
            for (int j = 0; j < tr.get(i).size(); ++j) {
                int a = tr.get(i).get(j).a;
                if (st.contains(a))
                    isDet = false;
                else
                    st.add(a);
            }
            if (st.size() != sza)
                isComplete = false;
        }
        if (!isDet)
            error("E5: FSA is nondeterministic");

        for (int i = 0; i < szs; ++i) {
            for (int j = 0; j < tr.get(i).size(); ++j) {
                String curAlpha;
                int cur = tr.get(i).get(j).s;
                if (r.get(0).get(i).get(cur).isEmpty())
                    curAlpha = this.alpha.get(tr.get(i).get(j).a + 1);
                else
                    curAlpha = r.get(0).get(i).get(cur) + "|" + this.alpha.get(tr.get(i).get(j).a + 1);
                r.get(0).get(i).set(cur, curAlpha);
            }

            for (int j = 0; j < szs; ++j) {
                if (i == j) {
                    String curAlpha;
                    if (r.get(0).get(i).get(j).isEmpty())
                        curAlpha = "eps";
                    else
                        curAlpha = r.get(0).get(i).get(j) + "|eps";
                    r.get(0).get(i).set(j, curAlpha);
                } else if (r.get(0).get(i).get(j).isEmpty()) {
                    String curAlpha = "{}";
                    r.get(0).get(i).set(j, curAlpha);
                }
            }
        }

        for (int k = 1; k <= szs; ++k) {
            for (int i = 0; i < szs; ++i) {
                for (int j = 0; j < szs; ++j) {
                    String cur = "(" + r.get(k-1).get(i).get(k-1) + ")(" + r.get(k-1).get(k-1).get(k-1) + ")*(" + r.get(k-1).get(k-1).get(j) + ")|(" + r.get(k-1).get(i).get(j) + ")";
                    r.get(k).get(i).set(j, cur);
                }
            }
        }

        String ans = new String();

        for (int i = 0; i < szs; ++i) {
            if (isFinal.contains(i)) {
                if(ans.isEmpty())
                    ans = r.get(szs).get(initialStateID).get(i);
                else
                    ans = ans + "|" + r.get(szs).get(initialStateID).get(i);
            }
        }

        if(ans.isEmpty())
            ans = "{}";

        out.write(ans);

        out.close();
    }
}

public class FSAtoREGEX {
    public static void main(String[] args) throws IOException {
        FileReader inp = new FileReader("input.txt");
        FileWriter out = new FileWriter("output.txt");
        Scanner in = new Scanner(inp);

        String states = in.nextLine();
        String alpha = in.nextLine();
        String initialState = in.nextLine();
        String finalState = in.nextLine();
        String transitions = in.nextLine();
        FSM fsm = new FSM(states, alpha, initialState, finalState, transitions, out);
    }
}
