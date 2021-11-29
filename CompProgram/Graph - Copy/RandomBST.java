public class RandomBST {
    class Node
    {
        int key;
        double balancedValue;
        Node left, right;
     
        public Node(int key, double balancedValue)
        {
            this.key = key;
            this.balancedValue = balancedValue;
            left = right = null;
        }
    }

    Node root;

    public boolean contains(int i) {
        return contains(i,root);
    }

    private boolean contains(int i, Node node) {
        if (node == null) {
            return false;
        }
        if (i == node.key) {
            return true;
        }
        if (i < node.key)
        {
            return contains(i, node.left);
        }
        else
        { 
            return contains(i, node.right);
        }
    }

    private Node rotateLeft (Node x) {
        Node y = x.right;
        Node T2 = y.left;
 
        // Perform rotation
        y.left = x;
        x.right = T2;

        return y;
    }

    private Node rotateRight (Node y) {
        Node x = y.left;
        Node T2 = x.right;
 
        // Perform rotation
        x.right = y;
        y.left = T2;
 
        return x;
    }
    
    public Node insert(Node node, int key) {
        double balancedValue = generateBalancedValue();
        if (node == null)
            return (new Node(key,balancedValue));
 
        if (key < node.key) {
            node.left = insert(node.left, key);
            if(node.balancedValue < balancedValue) {
                return rotateRight(node);
            }
        }
        else if (key > node.key) {
            node.right = insert(node.right, key);
            if(node.balancedValue < balancedValue) {
                return rotateLeft(node);
            }
        }   
        return node;
    }
    
    private double generateBalancedValue() {
        return Math.random();
    }

    public int getHeight() {
        return height(root);
    }

    private int height(Node node)
    {
        if (node == null) {
            return 0;
        }
        else
        {
            int lHeight = height(node.left);
            int rHeight = height(node.right);
  
            if (lHeight > rHeight)
                return (lHeight + 1);
             else
                return (rHeight + 1);
        }
    }

    public void preOrder() {
        preOrder(root);
    }

    private void preOrder(Node node)
    {
        if (node == null)
            return;
        System.out.print(node.key + " ");
        preOrder(node.left);
        preOrder(node.right);
    }

    public static void main(String[] args) {
        double avg = 0;
        for(int j=0;j<1000;j++) {
            RandomBST bst = new RandomBST();
            for(int i=0;i<1000;i++) {
                bst.root = bst.insert(bst.root,i);
            }
            avg += bst.getHeight();
        }
        avg /= 1000.0;
        System.out.printf("Average Height = %f\n",avg);
    }
}
