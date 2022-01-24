package ornek;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Map;

import javax.faces.context.FacesContext;

public class SampleDB {

	private Collection<Book> books;
	private Map session;

	public SampleDB() {

		session = FacesContext.getCurrentInstance().getExternalContext()
				.getApplicationMap();

		// "bookDB" id'li database'i y�kle
		books = (Collection<Book>) session.get("bookDB");

		// Database bo�sa �rnek verilerle ilklendir
		if (books == null) {

			books = new ArrayList<Book>();

			// -------------�RNEK K�TAPLAR-------------------
			books.add(new Book(1111, "Hans Bergsten", "Java Server Faces",
					null, true));
			books.add(new Book(3333, "Khaled Hosseini", "U�urtma Avc�s�", null,
					true));
			books.add(new Book(4444, "Dan Brown", "Cehennem", null, true));
			books.add(new Book(5555, "Adam Fawer", "Olas�l�ks�z", null, true));
			books.add(new Book(6666, "Orhan Pamuk", "Kar", null, true));
			books.add(new Book(7777, "J.R.R. Tolkien", "Hobbit", null, true));
			books.add(new Book(8888, "Adolf Hitler", "Kavgam", null, true));
			books.add(new Book(9999, "Kevin McNeish",
					"Flying with Objective C", null, true));

			session.put("bookDB", books);
		}
	}

	/**
	 * Database'den kitaplar� al
	 */
	private void loadData() {
		books = (Collection<Book>) session.get("bookDB");
	}

	/**
	 * �lgili kitab� database'e kaydet
	 * 
	 * @param book
	 */
	public void saveToDB(Book book) {
		// database'den kitaplar� y�kle
		loadData();

		ArrayList<Book> list = (ArrayList<Book>) books;
		int i;

		// e�er zaten b�yle bir kitap varsa g�ncelle
		for (i = 0; i < list.size(); i++) {
			if (list.get(i).getId() == book.getId()) {
				list.remove(i);
				break;
			}
		}

		list.add(i, book);
		books = list;

		// database'e kaydet
		session.put("bookDB", books);
	}

	/**
	 * Database'den ID ile kitap bilgilerini al
	 * 
	 * @param id
	 * @return
	 */
	public Book loadBookById(long id) {
		loadData();

		for (Book iter : books) {
			if (iter.getId() == id) {
				return iter;
			}
		}
		return null;
	}

	/**
	 * ID ile kitap sil
	 * 
	 * @param id
	 */
	public void deleteBookById(long id) {
		loadData();

		for (Book iter : books) {
			if (iter.getId() == id) {
				books.remove(iter);
				break;
			}
		}

		// database'e kaydet
		session.put("bookDB", books);
	}

	/**
	 * T�m kitaplar� al
	 * 
	 * @return
	 */
	public Collection<Book> getAllBooks() {
		loadData();
		return books;
	}
}