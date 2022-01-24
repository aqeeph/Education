package coreservlets;

import java.io.*;
import javax.servlet.*;
import javax.servlet.http.*;
import java.sql.*;

/** A variation of ConnectionPoolServlet that does NOT
 *  use connection pooling. Used to compare timing
 *  benefits of connection pooling.
 *  <P>
 *  Taken from Core Servlets and JavaServer Pages
 *  from Prentice Hall and Sun Microsystems Press,
 *  http://www.coreservlets.com/.
 *  &copy; 2000 Marty Hall; may be freely used or adapted.
 */

public class ConnectionPoolServlet3 extends HttpServlet {
  private String url, username, password;
  
  public void doGet(HttpServletRequest request,
                    HttpServletResponse response)
      throws ServletException, IOException {
    String table;
    String query =
      "SELECT firstname, lastname " +
      " FROM employees WHERE salary > 70000";
    try {
      Connection connection =
        DriverManager.getConnection(url, username, password);
      DBResults results =
        DatabaseUtilities.getQueryResults(connection,
                                          query, true);
      table = results.toHTMLTable("#FFAD00");
    } catch(Exception e) {
      table = "Exception: " + e;
    }
    response.setContentType("text/html");
    // Prevent the browser from caching the response. See
    // Section 7.2 of Core Servlets and JSP for details.
    response.setHeader("Pragma", "no-cache"); // HTTP 1.0
    response.setHeader("Cache-Control", "no-cache"); // HTTP 1.1
    PrintWriter out = response.getWriter();
    String title = "Connection Pool Test (*No* Pooling)";
    out.println(ServletUtilities.headWithTitle(title) +
                "<BODY BGCOLOR=\"#FDF5E6\">\n" +
                "<CENTER>\n" +
                table + "\n" +
                "</CENTER>\n</BODY></HTML>");
  }
  
  public void init() {
    try {
      int vendor = DriverUtilities.SYBASE;
      String driver = DriverUtilities.getDriver(vendor);
      Class.forName(driver);
      String host = "128.220.101.65";
      String dbName = "605741";
      url = DriverUtilities.makeURL(host, dbName, vendor);
      username = "hall";
      password = "hall";
    } catch(ClassNotFoundException e) {
      System.err.println("Error initializing: " + e);
      getServletContext().log("Error initializing: " + e);
    }
  }
}
