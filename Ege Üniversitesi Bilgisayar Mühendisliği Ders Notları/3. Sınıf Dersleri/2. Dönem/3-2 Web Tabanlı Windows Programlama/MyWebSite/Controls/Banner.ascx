<%@ Control Language="C#" AutoEventWireup="true" CodeFile="Banner.ascx.cs" Inherits="Controls_Banner" %>
<asp:Panel ID="VerticalPanel" runat="server">
    <a href="http://bilmuh.ege.edu.tr" target="_blank" runat="server" id="VerticalLink">
        <asp:Image ID="Image1" runat="server" AlternateText="This is a sample banner." 
             ImageUrl="~/Images/Banner_120x240.jpg" />
    </a>
</asp:Panel>
<asp:Panel ID="HorizontalPanel" runat="server">
    <a href="http://bilmuh.ege.edu.tr" target="_blank" runat="server" id="HorizontalLink">
        <asp:Image ID="Image2" runat="server" AlternateText="This is a sample banner." 
             ImageUrl="~/Images/Banner_468x60.jpg" />
    </a>
</asp:Panel>

