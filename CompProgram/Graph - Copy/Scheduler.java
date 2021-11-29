import java.util.Arrays;

public class Scheduler {

    public static Graph getTestGraph() {
        Graph graph = new MatrixGraph(10, Graph.DIRECTED_GRAPH);
        graph.addEdge(0,4);
        graph.addEdge(7,4);
        graph.addEdge(7,0);
        graph.addEdge(7,6);
        graph.addEdge(3,6);
        graph.addEdge(3,8);
        graph.addEdge(6,1);
        graph.addEdge(8,1);
        graph.addEdge(5,3);
        graph.addEdge(5,2);
        graph.addEdge(8,9);
        graph.addEdge(2,9);
        graph.addEdge(2,8);
        return graph;
    }

    public static void printGraph (Graph g) {
        for(int i=0;i<g.numVertices();i++) {
            System.out.println(i + " : " + Arrays.toString(g.outNeighbours(i)));
        }
    }

    public static void main(String[] args) {
        Graph testGraph = getTestGraph();
        printGraph(testGraph);
    }
}
