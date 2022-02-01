<%@ Page Title="Reviews Grouped by Genre" Language="C#" MasterPageFile="~/MasterPages/Frontend.master" AutoEventWireup="true" CodeFile="AllByGenre.aspx.cs" Inherits="Reviews_Default" %>

<%--Please email to me@mywebsite.com for copyright.--%>

<asp:Content ID="Content1" ContentPlaceHolderID="head" Runat="Server">
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="cpMainContent" Runat="Server"> 
    <asp:Repeater ID="Repeater1" runat="server">
        <ItemTemplate>
            <h3>
                <asp:Literal ID="Literal1" Text='<%# Eval("Name") %>' runat="server">
                </asp:Literal>
            </h3>
            <asp:BulletedList ID="reviewListBulletedList" runat="server" DataSource='<%#Eval("Reviews")%>' DataTextField="Title">
            </asp:BulletedList>
        </ItemTemplate>
    </asp:Repeater>
</asp:Content>

