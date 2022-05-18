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
    private ArrayList<String> states, alpha, finalState, transitions, warnings;
    private String initialState;
    private Integer initialStateID;
    private ArrayList<ArrayList<Edge>> tr;
    private ArrayList<ArrayList<Edge>> tr2way;
    private HashMap<String, Integer> stateToInt, alphaToInt;
    private HashSet<Integer> isFinal;
    private FileWriter out;
    private Integer szs, sza;
    private Boolean used[];

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
        if (!states.matches("states=\\[.*\\]$") || !alpha.matches("alpha=\\[.*\\]$")
            || !initialState.matches("init\\.st=\\[.*\\]$")
            || !finalState.matches("fin\\.st=\\[.*\\]$") || !transitions.matches("trans=\\[.*\\]$"))
            error("E5: Input file is malformed");

        this.states = new ArrayList<String>(Arrays.asList(states.split("states=\\[|,|\\]")));
        this.alpha = new ArrayList<String>(Arrays.asList(alpha.split("alpha=\\[|,|\\]")));
        String[] temp = initialState.split("init\\.st=\\[|\\]");
        if (temp.length <= 1)
            error("E4: Initial state is not defined");
        this.initialState = temp[1];
        this.finalState =
            new ArrayList<String>(Arrays.asList(finalState.split("fin\\.st=\\[|,|\\]")));
        this.transitions =
            new ArrayList<String>(Arrays.asList(transitions.split("trans=\\[|,|\\]")));

        this.szs = this.states.size();
        this.sza = this.alpha.size();

        stateToInt = new HashMap<String, Integer>();
        alphaToInt = new HashMap<String, Integer>();
        isFinal = new HashSet<Integer>();
        tr = new ArrayList<ArrayList<Edge>>();
        tr2way = new ArrayList<ArrayList<Edge>>();
        for (int i = 0; i < this.states.size(); ++i) {
            tr.add(new ArrayList<Edge>());
            tr2way.add(new ArrayList<Edge>());
        }
        warnings = new ArrayList<String>();
        used = new Boolean[szs];

        for (int i = 1; i < szs; ++i) this.stateToInt.put(this.states.get(i), i);

        if (this.finalState.size() <= 1)
            warnings.add("W1: Accepting state is not defined");

        for (int i = 1; i < this.finalState.size(); ++i) {
            if (this.stateToInt.containsKey(this.finalState.get(i)))
                isFinal.add(i);
            else
                error("E1: A state '" + this.finalState.get(i) + "' is not in the set of states");
        }

        for (int i = 1; i < sza; ++i) alphaToInt.put(this.alpha.get(i), i);

        for (int i = 1; i < this.transitions.size(); ++i) {
            String[] cur = this.transitions.get(i).split(">");

            if (cur.length != 3)
                error("E5: Input file is malformed");

            if (!this.stateToInt.containsKey(cur[0]))
                error("E1: A state '" + cur[0] + "' is not in the set of states");

            if (!this.alphaToInt.containsKey(cur[1]))
                error("E3: A transition '" + cur[1] + "' is not represented in the alphabet");

            if (!this.stateToInt.containsKey(cur[2]))
                error("E1: A state '" + cur[2] + "' is not in the set of states");

            int l = stateToInt.get(cur[0]), m = alphaToInt.get(cur[1]), r = stateToInt.get(cur[2]);
            tr.get(l).add(new Edge(r, m));
            tr2way.get(l).add(new Edge(r, m));
            tr2way.get(r).add(new Edge(l, m));
        }

        if (stateToInt.containsKey(this.initialState))
            initialStateID = stateToInt.get(this.initialState);
        else
            error("E4: Initial state is not defined");

        int components = 0;
        for (int i = 1; i < szs; ++i) used[i] = false;
        for (int i = 1; i < szs; ++i) {
            if (!used[i]) {
                dfs2way(i);
                components++;
            }
        }

        if (components > 1)
            error("E2: Some states are disjoint");

        for (int i = 1; i < szs; ++i) used[i] = false;

        int cnt = dfs(initialStateID);
        if (cnt != szs - 1)
            warnings.add("W2: Some states are not reachable from the initial state");

        Boolean isDet = true, isComplete = true;
        for (int i = 1; i < szs; ++i) {
            HashSet<Integer> st = new HashSet<Integer>();
            for (int j = 0; j < tr.get(i).size(); ++j) {
                int a = tr.get(i).get(j).a;
                if (st.contains(a))
                    isDet = false;
                else
                    st.add(a);
            }
            if (st.size() != this.alpha.size() - 1)
                isComplete = false;
        }
        if (!isDet)
            warnings.add("W3: FSA is nondeterministic");

        if (isComplete)
            out.write("FSA is complete");
        else
            out.write("FSA is incomplete");

        if (warnings.size() > 0)
            out.write("\nWarning: ");
        for (int i = 0; i < warnings.size(); ++i) {
            out.write("\n" + warnings.get(i));
        }

        out.close();
    }
}

public class FSA {
    public static void main(String[] args) throws IOException {
        FileReader inp = new FileReader("fsa.txt");
        FileWriter out = new FileWriter("result.txt");
        Scanner in = new Scanner(inp);

        String states = in.nextLine();
        String alpha = in.nextLine();
        String initialState = in.nextLine();
        String finalState = in.nextLine();
        String transitions = in.nextLine();
        FSM fsm = new FSM(states, alpha, initialState, finalState, transitions, out);
    }
}
