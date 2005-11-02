<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    <xsl:template match="document">
        <html>
            <head>
                <title>Project Overview</title>
                <base href="http://utgs.berlios.de/www/"/>
            </head>
            <body background="utgs-bg.jpg">
                <table border="0" cellspacing="0" cellpadding="0" width="100%" height="64">
                    <tr valign="top" bgcolor="#3c8fdc" background="utgslogobg.jpg">
                        <td width="128px">
                            <table width="128px"/>
                        </td>
                        <td valign="middle" width="99%">
                            <img src="utgslogo.png"/>
                        </td>
                    </tr>
                    <tr>
                        <td colspan="2" bgcolor="black">
                            <table height="1px"/>
                        </td>
                    </tr>
                </table>
                
                <table width="80%" align="center" bgcolor="#3c8fdc" cellpadding="0">
                    <tr>
                        <td bgcolor="white">
                            <table cellpadding="10" width="100%" background="utgs-bg2.jpg">
                                <tr>
                                    <td>
                                        <h2><xsl:apply-templates select="title"/></h2>
                                        <h4><xsl:apply-templates select="sub-title"/></h4>
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        <h3>Table Of Contents.</h3>
                                        <p>On <a href="#">this</a> page:
                                            <ul>
                                                <xsl:for-each select="chapters/chapter">
                                                    <xsl:variable name="chapterNo">
                                                        <xsl:number count="chapter"/>
                                                    </xsl:variable>
                                                    <li>
                                                        <xsl:element name="a">
                                                            <xsl:attribute name="href">
                                                                <xsl:text>#</xsl:text>
                                                                <xsl:value-of select="./name"/>
                                                            </xsl:attribute>
                                                            <xsl:text>Chapter </xsl:text>
                                                            <xsl:value-of select="$chapterNo"/>
                                                            <xsl:text> : </xsl:text>
                                                            <xsl:value-of select="./title"/>
                                                        </xsl:element>
                                                    </li>
                                                </xsl:for-each>
                                            </ul>
                                        </p>
                                        <xsl:if test="chapters/toc">
                                            <p>Other pages:
                                                <ul>
                                                    <xsl:for-each select="chapters/toc/*">
                                                        <li>
                                                            <xsl:element name="a">
                                                                <xsl:attribute name="href">
                                                                    <xsl:apply-templates select="./href"/>
                                                                </xsl:attribute>
                                                                <xsl:apply-templates select="./title"/>
                                                            </xsl:element>
                                                        </li>
                                                    </xsl:for-each>
                                                </ul>
                                            </p>
                                        </xsl:if>
                                    </td>
                                </tr>
                                <tr height="64px"/>
                                <tr>
                                    <td align="right" valign="bottom" width="99%">
                                        See <a href="http://utgs.berlios.de">UTGS home page</a>.<br/>
                                        See <a href="../COPYING">COPYING</a> for license information.<br/>
                                        Copyright (c) 2005, Radoslaw Kolasinski (<a href="mailto:koolas@gmail.com">koolas@gmail.com</a>) All rights reserved.<br/>
                                    </td>
                                </tr>
                                <tr height="64px"/>
                            </table>
                        <xsl:apply-templates select="chapters/chapter"/>
                        </td>
                    </tr>
                </table>
                
                <table>
                    <tr height="480px"/>
                </table>
            </body>
        </html>
    </xsl:template>


    <xsl:template name="ResolveLink">
        <xsl:variable name="linkTo" select="."/>
        <xsl:choose>
            <xsl:when test="/document/links/link[@name=$linkTo]">
                <xsl:element name="a">
                    <xsl:attribute name="href">
                        <xsl:value-of select="/document/links/link[@name=$linkTo]"/>
                    </xsl:attribute>
                    <i>
                        <xsl:value-of select="."/>
                    </i>
                </xsl:element>
            </xsl:when>
            <xsl:when test="/document/chapters/chapter/title=$linkTo">
                <xsl:element name="a">
                    <xsl:attribute name="href">
                        <xsl:text>#</xsl:text>
                        <xsl:for-each select="/document/chapters/chapter">
                            <xsl:if test="./title=$linkTo">
                                <xsl:value-of select="./name"/>
                            </xsl:if>
                        </xsl:for-each>
                    </xsl:attribute>
                    <i>
                        <font color="blue">
                            <xsl:value-of select="."/>
                        </font>
                    </i>
                </xsl:element>
            </xsl:when>
            <xsl:otherwise>
                <font color="darkblue">
                    <i>
                        <xsl:apply-templates/>
                    </i>
                </font>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>


    <xsl:template match="document/chapters/chapter">
        <xsl:variable name="chapterNo">
            <xsl:number count="chapter"/>
        </xsl:variable>
        <table width="100%" cellspacing="0" background="utgs-bg1.jpg">
            <tr height="96px">
                <td bgcolor="#dc8f3c" width="80%" background="utgschapterbg.jpg">
                    <xsl:element name="h2">
                        <xsl:attribute name="id">
                            <xsl:value-of select="name"/>
                        </xsl:attribute>
                        <xsl:text>Chapter </xsl:text>
                        <xsl:value-of select="$chapterNo"/>
                        <xsl:text> : </xsl:text>
                        <xsl:element name="a">
                            <xsl:attribute name="href">
                                <xsl:text>#</xsl:text>
                                <xsl:value-of select="name"/>
                            </xsl:attribute>
                            <xsl:apply-templates select="title"/>
                        </xsl:element>
                    </xsl:element>
                </td>
                <td width="46px" background="utgschapter.jpg"/>
                <td align="center">
                    <a href="#">Back to ToC</a>
                </td>
            </tr>
        </table>
        <table cellpadding="10" width="100%" background="utgs-bg2.jpg">
            <tr>
                <td>
                    <xsl:apply-templates select="body"/>
                </td>
            </tr>
            <tr>
                <td>
                    <a href="#">Back to ToC</a>
                </td>
            </tr>
            <tr height="128px"/>
        </table>
    </xsl:template>

    <xsl:template match="ul">
        <center>
            <br/>
            <table bgcolor="lightgrey" width="80%" noborder="1">
                <xsl:for-each select="./li">
                    <tr>
                        <td width="300px" bgcolor="white">
                            <xsl:for-each select="./n">
                                <xsl:call-template name="ResolveLink"/>
                            </xsl:for-each>
                        </td>
                        <td bgcolor="white">
                            <xsl:apply-templates select="./d"/>
                        </td>
                    </tr>
                </xsl:for-each>
            </table>
            <br/>
        </center>
    </xsl:template>

    <xsl:template match="ll">
        <ul>
            <xsl:for-each select="./li">
                <li>
                    <xsl:apply-templates/>
                </li>
            </xsl:for-each>
        </ul>
    </xsl:template>

    <xsl:template match="i">
        <xsl:call-template name="ResolveLink"/>
    </xsl:template>
    
    <xsl:template match="b">
        <b>
            <font color="darkred">
                <xsl:apply-templates/>
            </font>
        </b>
    </xsl:template>
    
    
    <xsl:template match="p">
        <p>
            <xsl:apply-templates/>
        </p>
    </xsl:template>
    
    <xsl:template match="q">
        <font color="darkmagenta">
            <q>
                <xsl:apply-templates/>
            </q>
        </font>
    </xsl:template>

    <xsl:template match="br">
        <br/>
    </xsl:template>
    
    <xsl:template match="hr">
        <hr/>
    </xsl:template>

    <xsl:template match="image">
        <center>
            <xsl:element name="img">
                <xsl:attribute name="src">
                    <xsl:value-of select="@src"/>
                </xsl:attribute>
            </xsl:element>
        </center>
    </xsl:template>

    <xsl:template match="tip">
        <center>
            <br/>
            <table bgcolor="yellow" width="80%">
                <tr>
                    <td bgcolor="lightyellow">
                        <table>
                            <tr height="5px"/>
                            <tr>
                                <td width="5px"/>
                                <td><xsl:apply-templates/></td>
                                <td width="5px"/>
                            </tr>
                            <tr height="5px"/>
                        </table>
                    </td>
                </tr>
            </table>
            <br/>
        </center>
    </xsl:template>
    
    <xsl:template match="link">
        <xsl:element name="a">
            <xsl:attribute name="href">
                <xsl:apply-templates select="./href"/>
            </xsl:attribute>
            <xsl:apply-templates select="./title"/>
        </xsl:element>
    </xsl:template>

    <xsl:template match="mailto">
        <xsl:element name="a">
            <xsl:attribute name="href">
                <xsl:text>mailto:</xsl:text>
                <xsl:value-of select="."/>
            </xsl:attribute>
            <xsl:value-of select="."/> 
        </xsl:element>
    </xsl:template>

    <xsl:template name="codePartXML" match="code[@lang='xml']/*">
        <code>
            <font color="darkcyan">
                <b>
                    <xsl:text>&lt;</xsl:text>
                    <xsl:value-of select="name()"/>
                </b>
            </font>
            <xsl:if test="@*">
                <xsl:text> </xsl:text>
            </xsl:if>
            <xsl:for-each select="@*">
                <b>
                    <font color="green">
                        <xsl:value-of select="name()"/>
                    </font>
                    <xsl:text>=</xsl:text>
                    <font color="darkmagenta">
                        <xsl:text>"</xsl:text>
                        <xsl:value-of select="."/>
                        <xsl:text>"</xsl:text>
                    </font>
                    <xsl:text> </xsl:text>
                </b>
            </xsl:for-each>
            <xsl:choose>
                <xsl:when test="./*">
                    <font color="darkcyan">
                        <b>
                            <xsl:text>&gt;</xsl:text>
                        </b>
                    </font>
                    <xsl:for-each select="./*">
                        <table cellspacing="4" cellpadding="0">
                            <tr>
                                <td width="16px"/>
                                <td>
                                    <xsl:choose>
                                        <xsl:when test="name() = 'fixed-comment'">
                                            <font color="blue">
                                                <xsl:text>&lt;!-- </xsl:text>
                                                <xmp>
                                                    <xsl:value-of select="."/>
                                                </xmp>
                                                <xsl:text> --&gt;</xsl:text>
                                            </font>
                                        </xsl:when>
                                        <xsl:when test="name() = 'comment'">
                                            <font color="blue">
                                                <xsl:text>&lt;!-- </xsl:text>
                                                <xsl:value-of select="."/>
                                                <xsl:text> --&gt;</xsl:text>
                                            </font>
                                        </xsl:when>
                                        <xsl:otherwise>
                                            <xsl:call-template name="codePartXML"/>
                                        </xsl:otherwise>
                                    </xsl:choose>
                                </td>
                            </tr>
                        </table>
                    </xsl:for-each>
                    <font color="darkcyan">
                        <b>
                            <xsl:text>&lt;/</xsl:text>
                            <xsl:value-of select="name()"/>
                            <xsl:text>&gt;</xsl:text>
                        </b>
                    </font>
                </xsl:when>
                <xsl:when test="node()">
                    <font color="darkcyan">
                        <b>
                            <xsl:text>&gt;</xsl:text>
                        </b>
                    </font>
                    <xsl:value-of select="node()"/>
                    <font color="darkcyan">
                        <b>
                            <xsl:text>&lt;/</xsl:text>
                            <xsl:value-of select="name()"/>
                            <xsl:text>&gt;</xsl:text>
                        </b>
                    </font>
                </xsl:when>
                <xsl:otherwise>
                    <font color="darkcyan">
                        <b>
                            <xsl:text>/&gt;</xsl:text>
                        </b>
                    </font>
                </xsl:otherwise>
            </xsl:choose>
        </code>
    </xsl:template>
    
    <xsl:template name="codePartText" match="code[@lang='text']">
        <code>
            <xsl:apply-templates/>
        </code>
    </xsl:template>
    
    <xsl:template name="codePartCpp" match="code[@lang='cpp']">
        <code>
            <xsl:apply-templates/>
        </code>
    </xsl:template>

    <xsl:template match="code[@lang='cpp']/s">
        <font color="blue">
            <xsl:apply-templates/>
        </font>
    </xsl:template>
    
    <xsl:template match="code[@lang='cpp']/v">
        <font color="magenta">
            <xsl:apply-templates/>
        </font>
    </xsl:template>
    
    <xsl:template match="code[@lang='cpp']/o">
        <font color="black">
            <xsl:apply-templates/>
        </font>
    </xsl:template>

    <xsl:template match="pre">
        <pre>
            <xsl:apply-templates/>
        </pre>
    </xsl:template>

</xsl:stylesheet>
