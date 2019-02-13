import java.util.ArrayList;
public class LibraryTree {
	
	public LibraryNode primaryRoot;		//root of the primary B+ tree
	public LibraryNode secondaryRoot;	//root of the secondary B+ tree
	public LibraryTree(Integer order)
	{
		LibraryNode.order = order;
		primaryRoot = new LibraryNodeLeaf(null);
		primaryRoot.level = 0;
		secondaryRoot = new LibraryNodeLeaf(null);
		secondaryRoot.level = 0;
	}
	
	public void addBook(CengBook book) {
		// add methods to fill both primary and secondary tree

		// Primary tree

		// Find correct leaf
		LibraryNode tempNode = goToLeaf(book);
		// Put data to the proper place in the leaf
		addBookToLeaf(tempNode, book);
		// Check if leaf overflows
		if (2 * primaryRoot.order < ((LibraryNodeLeaf) tempNode).bookCount()) {
			// Copy up
			tempNode = copyUp(tempNode);
			// Check if internal node overflows
			while (2 * primaryRoot.order < ((LibraryNodePrimaryIndex) tempNode).keyCount()) {
				// Push up
				tempNode = pushUp(tempNode);
			}
			// Check if tempNode is the new root
			if (tempNode.getParent() == null) {
				// Modify root with the new root
				primaryRoot = tempNode;
			}
		}

		// Secondary tree

		// Find correct leaf
		tempNode = goToLeaf2(book);
		// Put data to the proper place in the leaf
		addBookToLeaf2(tempNode, book);
		// Check if leaf overflows
		if (2 * secondaryRoot.order < ((LibraryNodeLeaf) tempNode).bookCount()) {
			// Copy up
			tempNode = copyUp2(tempNode);
			while (2 * secondaryRoot.order < ((LibraryNodeSecondaryIndex) tempNode).keyCount()) {
				// Push up
				tempNode = pushUp2(tempNode);
			}
			// Check if tempNode is the new root
			if (tempNode.getParent() == null) {
				// Modify root with the new root
				secondaryRoot = tempNode;
			}
		}


		return;
	}
	
	public CengBook searchBook(Integer key) {
		// add methods to find the book with the searched key in primary B+ tree
		// return value will not be tested, just print as the specicifications say
		Integer n;
		LibraryNode tempNode = primaryRoot;
		while (tempNode.getType() != LibraryNodeType.Leaf) {
			n = ((LibraryNodePrimaryIndex) tempNode).keyCount();
			System.out.println("<index>");
			for (int i = 0; i < n ; i++) {
				System.out.println(((LibraryNodePrimaryIndex) tempNode).keyAtIndex(i));
			}
			System.out.println("</index>");
			if (key < ((LibraryNodePrimaryIndex) tempNode).keyAtIndex(0)) {
				tempNode = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(0);
			}
			else if (((LibraryNodePrimaryIndex) tempNode).keyAtIndex(n - 1) <= key) {
				tempNode = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(n);
			}
			else {
				for (int i = 0; i <= n - 2; i++) {
					if (((LibraryNodePrimaryIndex) tempNode).keyAtIndex(i) <= key && key < ((LibraryNodePrimaryIndex) tempNode).keyAtIndex(i + 1)) {
						tempNode = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(i + 1);
						break;
					}
				}
			}

		}
		n = ((LibraryNodeLeaf) tempNode).bookCount();
		for (int i = 0; i < n; i++) {
			CengBook book = ((LibraryNodeLeaf) tempNode).bookAtIndex(i);
			if (book.key() == key) {
				System.out.println("<data>");
				System.out.println("<record>" + book.fullName() + "</record>");
				System.out.println("</data>");
				return book;
			}
		}
		System.out.println("No match for " + key);
		return null;
		}
	
	
	public void printPrimaryLibrary() {
		// add methods to print the primary B+ tree in Depth-first order
		dfs(primaryRoot);
		return;
	}
	
	public void printSecondaryLibrary() {
		// add methods to print the secondary B+ tree in Depth-first order
		dfs2(secondaryRoot);
		return;
	}
	
	// Extra functions if needed

	private LibraryNode goToLeaf(CengBook book) {
		Integer n;
		LibraryNode tempNode = primaryRoot;
		while (tempNode.getType() != LibraryNodeType.Leaf) {
			n = ((LibraryNodePrimaryIndex) tempNode).keyCount();
			if (book.key() < ((LibraryNodePrimaryIndex) tempNode).keyAtIndex(0)) {
				tempNode = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(0);
			}
			else if (((LibraryNodePrimaryIndex) tempNode).keyAtIndex(n - 1) <= book.key()) {
				tempNode = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(n);
			}
			else {
				for (int i = 0; i <= n - 2; i++) {
					if (((LibraryNodePrimaryIndex) tempNode).keyAtIndex(i) <= book.key() && book.key() < ((LibraryNodePrimaryIndex) tempNode).keyAtIndex(i + 1)) {
						tempNode = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(i + 1);
						break;
					}
				}
			}
		}
		return tempNode;
	}

	private void addBookToLeaf(LibraryNode tempNode, CengBook book) {
		Integer n = ((LibraryNodeLeaf) tempNode).bookCount();
		for (int i = 0; i < n; i++) {
			if (book.key() < ((LibraryNodeLeaf) tempNode).bookKeyAtIndex(i)) {
				((LibraryNodeLeaf) tempNode).addBook(i, book);
				return;
			}
		}
		((LibraryNodeLeaf) tempNode).addBook(n, book);
		return;
	}

	private LibraryNode copyUp(LibraryNode tempNode) {
		// Split Leaf
		Integer n = ((LibraryNodeLeaf) tempNode).bookCount();
		Integer midIndex = n / 2;
		Integer midKey = ((LibraryNodeLeaf) tempNode).bookKeyAtIndex(midIndex);
	
		LibraryNode newNode1 = new LibraryNodeLeaf(null);
		LibraryNode newNode2 = new LibraryNodeLeaf(null);

		LibraryNodePrimaryIndex parentOfTempNode = (LibraryNodePrimaryIndex) tempNode.getParent();

		for (int i = 0; i < midIndex; i++) {
			((LibraryNodeLeaf) newNode1).addBook(i, ((LibraryNodeLeaf) tempNode).bookAtIndex(i));
		}
		for (int i = midIndex; i < n; i++) {
			((LibraryNodeLeaf) newNode2).addBook(i - midIndex, ((LibraryNodeLeaf) tempNode).bookAtIndex(i));
		}

		if (parentOfTempNode == null) {
			parentOfTempNode = new LibraryNodePrimaryIndex(null);
			parentOfTempNode.addKey(0, midKey);
			parentOfTempNode.addChild(0, newNode1);
			parentOfTempNode.addChild(1, newNode2);
		}
		else {
			Integer childIndex = parentOfTempNode.findChildIndex(tempNode);
			parentOfTempNode.addKey(childIndex, midKey);
			parentOfTempNode.replaceChild(childIndex, newNode1);
			parentOfTempNode.addChild(childIndex + 1, newNode2);
		}

		newNode1.setParent((LibraryNode) parentOfTempNode);
		newNode2.setParent((LibraryNode) parentOfTempNode);

		return (LibraryNode) parentOfTempNode;

	}

	private LibraryNode pushUp(LibraryNode tempNode) {
		Integer n = ((LibraryNodePrimaryIndex) tempNode).keyCount();
		Integer midIndex = n / 2;
		Integer midKey = ((LibraryNodePrimaryIndex) tempNode).keyAtIndex(midIndex);
	
		LibraryNode newNode1 = new LibraryNodePrimaryIndex(null);
		LibraryNode newNode2 = new LibraryNodePrimaryIndex(null);

		LibraryNode child;

		LibraryNodePrimaryIndex parentOfTempNode = (LibraryNodePrimaryIndex) tempNode.getParent();

		for (int i = 0; i < midIndex; i++) {
			child = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(i);
			child.setParent(newNode1);
			((LibraryNodePrimaryIndex) newNode1).addChild(i, child);
			((LibraryNodePrimaryIndex) newNode1).addKey(i, ((LibraryNodePrimaryIndex) tempNode).keyAtIndex(i));
		}
		child = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(midIndex);
		child.setParent(newNode1);
		((LibraryNodePrimaryIndex) newNode1).addChild(midIndex, child);

		for (int i = midIndex + 1; i < n; i++) {
			child = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(i);
			child.setParent(newNode2);
			((LibraryNodePrimaryIndex) newNode2).addChild(i - midIndex - 1, child);
			((LibraryNodePrimaryIndex) newNode2).addKey(i - midIndex - 1, ((LibraryNodePrimaryIndex) tempNode).keyAtIndex(i));
		}
		child = ((LibraryNodePrimaryIndex) tempNode).getChildrenAt(n);
		child.setParent(newNode2);
		((LibraryNodePrimaryIndex) newNode2).addChild(n - midIndex - 1, child);

		// Push Up
		if (parentOfTempNode == null) {
			parentOfTempNode = new LibraryNodePrimaryIndex(null);
			parentOfTempNode.addKey(0, midKey);
			parentOfTempNode.addChild(0, newNode1);
			parentOfTempNode.addChild(1, newNode2);
		}
		else {
			Integer childIndex = parentOfTempNode.findChildIndex(tempNode);
			parentOfTempNode.addKey(childIndex, midKey);
			parentOfTempNode.replaceChild(childIndex, newNode1);
			parentOfTempNode.addChild(childIndex + 1, newNode2);
		}

		newNode1.setParent((LibraryNode) parentOfTempNode);
		newNode2.setParent((LibraryNode) parentOfTempNode);

		return (LibraryNode) parentOfTempNode;
		
	}

	private LibraryNode goToLeaf2(CengBook book) {
		Integer n;
		LibraryNode tempNode = secondaryRoot;
		while (tempNode.getType() != LibraryNodeType.Leaf) {
			n = ((LibraryNodeSecondaryIndex) tempNode).keyCount();
			if (((book.year() < ((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(0))) ||
			   ((book.year() == ((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(0)) &&
			   	(book.key() < ((LibraryNodeSecondaryIndex) tempNode).keyAtIndex(0)))) {
				tempNode = ((LibraryNodeSecondaryIndex) tempNode).getChildrenAt(0);
			}
			else if ((((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(n - 1) <= book.year()) ||
					((((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(n - 1) == book.year()) &&
					 (((LibraryNodeSecondaryIndex) tempNode).keyAtIndex(n - 1) <= book.key()))) {
				tempNode = ((LibraryNodeSecondaryIndex) tempNode).getChildrenAt(n);
			}
			else {
				for (int i = 0; i <= n - 2; i++) {
					if (((((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(i) <= book.year()) ||
						((((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(i) == book.year()) &&
						 (((LibraryNodeSecondaryIndex) tempNode).keyAtIndex(i) <= book.key()))) &&
						((((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(i + 1) > book.year()) ||
						((((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(i + 1) == book.year()) &&
						 (((LibraryNodeSecondaryIndex) tempNode).keyAtIndex(i + 1) > book.key())))) {
						tempNode = ((LibraryNodeSecondaryIndex) tempNode).getChildrenAt(i + 1);
						break;
					}
				}
			}
		}
		return tempNode;
	}

	private void addBookToLeaf2(LibraryNode tempNode, CengBook book) {
		Integer n = ((LibraryNodeLeaf) tempNode).bookCount();
		for (int i = 0; i < n; i++) {
			if ((book.year() < ((LibraryNodeLeaf) tempNode).bookYearAtIndex(i)) ||
			   ((book.year() == ((LibraryNodeLeaf) tempNode).bookYearAtIndex(i)) &&
			   	(book.key() < ((LibraryNodeLeaf) tempNode).bookKeyAtIndex(i)))) {
				((LibraryNodeLeaf) tempNode).addBook(i, book);
				return;
			}
		}
		((LibraryNodeLeaf) tempNode).addBook(n, book);
		return;
	}

	private LibraryNode copyUp2(LibraryNode tempNode) {
		// Split Leaf
		Integer n = ((LibraryNodeLeaf) tempNode).bookCount();
		Integer midIndex = n / 2;
		Integer midYear = ((LibraryNodeLeaf) tempNode).bookYearAtIndex(midIndex);
		Integer midKey = ((LibraryNodeLeaf) tempNode).bookKeyAtIndex(midIndex);
	
		LibraryNode newNode1 = new LibraryNodeLeaf(null);
		LibraryNode newNode2 = new LibraryNodeLeaf(null);

		LibraryNodeSecondaryIndex parentOfTempNode = (LibraryNodeSecondaryIndex) tempNode.getParent();

		for (int i = 0; i < midIndex; i++) {
			((LibraryNodeLeaf) newNode1).addBook(i, ((LibraryNodeLeaf) tempNode).bookAtIndex(i));
		}
		for (int i = midIndex; i < n; i++) {
			((LibraryNodeLeaf) newNode2).addBook(i - midIndex, ((LibraryNodeLeaf) tempNode).bookAtIndex(i));
		}


		// Copy Up
		if (parentOfTempNode == null) {
			parentOfTempNode = new LibraryNodeSecondaryIndex(null);
			parentOfTempNode.addYear(0, midYear);
			parentOfTempNode.addKey(0, midKey);
			parentOfTempNode.addChild(0, newNode1);
			parentOfTempNode.addChild(1, newNode2);
		}
		else {
			Integer childIndex = parentOfTempNode.findChildIndex(tempNode);
			parentOfTempNode.addYear(childIndex, midYear);
			parentOfTempNode.addKey(childIndex, midKey);
			parentOfTempNode.replaceChild(childIndex, newNode1);
			parentOfTempNode.addChild(childIndex + 1, newNode2);
		}

		newNode1.setParent((LibraryNode) parentOfTempNode);
		newNode2.setParent((LibraryNode) parentOfTempNode);

		return (LibraryNode) parentOfTempNode;

	}

	private LibraryNode pushUp2(LibraryNode tempNode) {
		// Split Internal node
		Integer n = ((LibraryNodeSecondaryIndex) tempNode).keyCount();
		Integer midIndex = n / 2;
		Integer midYear = ((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(midIndex);
		Integer midKey = ((LibraryNodeSecondaryIndex) tempNode).keyAtIndex(midIndex);
	
		LibraryNode newNode1 = new LibraryNodeSecondaryIndex(null);
		LibraryNode newNode2 = new LibraryNodeSecondaryIndex(null);

		LibraryNodeSecondaryIndex parentOfTempNode = (LibraryNodeSecondaryIndex) tempNode.getParent();

		LibraryNode child;

		for (int i = 0; i < midIndex; i++) {
			child = ((LibraryNodeSecondaryIndex) tempNode).getChildrenAt(i);
			child.setParent(newNode1);
			((LibraryNodeSecondaryIndex) newNode1).addChild(i, child);
			((LibraryNodeSecondaryIndex) newNode1).addYear(i, ((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(i));
			((LibraryNodeSecondaryIndex) newNode1).addKey(i, ((LibraryNodeSecondaryIndex) tempNode).keyAtIndex(i));
		}
		child = ((LibraryNodeSecondaryIndex) tempNode).getChildrenAt(midIndex);
		child.setParent(newNode1);
		((LibraryNodeSecondaryIndex) newNode1).addChild(midIndex, child);

		for (int i = midIndex + 1; i < n; i++) {
			child = ((LibraryNodeSecondaryIndex) tempNode).getChildrenAt(i);
			child.setParent(newNode2);
			((LibraryNodeSecondaryIndex) newNode2).addChild(i - midIndex - 1, child);
			((LibraryNodeSecondaryIndex) newNode2).addYear(i - midIndex - 1, ((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(i));
			((LibraryNodeSecondaryIndex) newNode2).addKey(i - midIndex - 1, ((LibraryNodeSecondaryIndex) tempNode).keyAtIndex(i));
		}
		child = ((LibraryNodeSecondaryIndex) tempNode).getChildrenAt(n);
		child.setParent(newNode2);
		((LibraryNodeSecondaryIndex) newNode2).addChild(n - midIndex - 1, child);

		// Copy Up
		if (parentOfTempNode == null) {
			parentOfTempNode = new LibraryNodeSecondaryIndex(null);
			parentOfTempNode.addYear(0, midYear);
			parentOfTempNode.addKey(0, midKey);
			parentOfTempNode.addChild(0, newNode1);
			parentOfTempNode.addChild(1, newNode2);
		}
		else {
			Integer childIndex = parentOfTempNode.findChildIndex(tempNode);
			parentOfTempNode.addYear(childIndex, midYear);
			parentOfTempNode.addKey(childIndex, midKey);
			parentOfTempNode.replaceChild(childIndex, newNode1);
			parentOfTempNode.addChild(childIndex + 1, newNode2);
		}

		newNode1.setParent((LibraryNode) parentOfTempNode);
		newNode2.setParent((LibraryNode) parentOfTempNode);

		return (LibraryNode) parentOfTempNode;

	}

	private void dfs(LibraryNode tempNode) {
		Integer n;
		if (tempNode.getType() == LibraryNodeType.Leaf) {
			n = ((LibraryNodeLeaf) tempNode).bookCount();
			System.out.println("<data>");
			for (int i = 0; i < n; i++) {
				System.out.println("<record>" + ((LibraryNodeLeaf) tempNode).bookAtIndex(i).fullName() + "</record>");
			}
			System.out.println("</data>");
			return;
		}
		n = ((LibraryNodePrimaryIndex) tempNode).keyCount();
		System.out.println("<index>");
		for (int i = 0; i < n; i++) {
			System.out.println(((LibraryNodePrimaryIndex) tempNode).keyAtIndex(i));
		}
		System.out.println("</index>");
		for (int i = 0; i < n + 1; i++) {
			dfs(((LibraryNodePrimaryIndex) tempNode).getChildrenAt(i));
		}
		return;
	}

	private void dfs2(LibraryNode tempNode) {
		Integer n;
		if (tempNode.getType() == LibraryNodeType.Leaf) {
			n = ((LibraryNodeLeaf) tempNode).bookCount();
			System.out.println("<data>");
			for (int i = 0; i < n; i++) {
				System.out.println("<record>" + ((LibraryNodeLeaf) tempNode).bookAtIndex(i).fullName() + "</record>");
			}
			System.out.println("</data>");
			return;
		}
		n = ((LibraryNodeSecondaryIndex) tempNode).keyCount();
		System.out.println("<index>");
		for (int i = 0; i < n; i++) {
			System.out.println(((LibraryNodeSecondaryIndex) tempNode).yearAtIndex(i) + "|" + ((LibraryNodeSecondaryIndex) tempNode).keyAtIndex(i));
		}
		System.out.println("</index>");
		for (int i = 0; i < n + 1; i++) {
			dfs2(((LibraryNodeSecondaryIndex) tempNode).getChildrenAt(i));
		}
		return;
	}
}


