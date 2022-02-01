<%@ Page Title="Web Services" Language="C#" MasterPageFile="~/MasterPages/Frontend.master" AutoEventWireup="true" CodeFile="WebServices.aspx.cs" Inherits="_WebServices" %>

<%--Please email to me@mywebsite.com for copyright.--%>

<asp:Content ID="Content1" ContentPlaceHolderID="head" Runat="Server">
</asp:Content>
<asp:Content ID="Content2" ContentPlaceHolderID="cpMainContent" Runat="Server"> 
    <asp:ScriptManagerProxy ID="ScriptManagerProxy1" runat="server">
        <Services>
            <asp:ServiceReference Path="~/WebServices/NameService.asmx" />
        </Services>
    </asp:ScriptManagerProxy>
    <input id="yourNameText" type="text" />
    <input id="sayHelloButton" type="button" value="Say Hello" />
    <input id="sayHelloPageMethod" type="button" value="Say Hello with a Page Method" />
    <script type="text/javascript">
        function HelloWorld() {
            var yourName = $get('yourNameText').value;
            NameService.HelloWorld(yourName, HelloWorldCallback);
        }
        function HelloWorldPageMethod() {
            var yourName = $get('yourNameText').value;
            PageMethods.HelloWorld(yourName, HelloWorldCallback);
        }
        function HelloWorldCallback(result) {
            alert(result);
        }
        $addHandler($get('sayHelloButton'), 'click', HelloWorld);
        $addHandler($get('sayHelloPageMethod'), 'click', HelloWorldPageMethod);
    </script>
</asp:Content>

