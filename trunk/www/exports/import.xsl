<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

    <xsl:template match="xml">
        <html>
            <head>
                <title><xsl:value-of select="title"/></title>
            </head>
            <body>
                <h1><xsl:value-of select="title"/></h1>
                <p>
                    <xsl:value-of select="./description"/>
                </p>
                <table height="20pt"/>
                <xsl:apply-templates/>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="*">
    </xsl:template>

    <xsl:template name="do_block" match="block">
        <h2>Member Functions:</h2>
        <hr/>
        <blockquote>
            <xsl:for-each select="chunk">
                <h2>
                    <xsl:value-of select="@id"/>
                </h2>
                <h4>
                    <xsl:value-of select="./type-name"/>
                </h4>
                <p>
                    <xsl:value-of select="./description"/>
                </p>
                <table height="20pt"/>
            </xsl:for-each>
        </blockquote>
        <h2>Constants:</h2>
        <hr/>
        <blockquote>
            <xsl:for-each select="string">
                <h2>
                    <xsl:value-of select="@id"/>
                </h2>
                <h4>
                    Constant String
                </h4>
                <p>
                    <xsl:value-of select="./description"/>
                </p>
                <table height="20pt"/>
            </xsl:for-each>
        </blockquote>
        <blockquote>
            <xsl:for-each select="integer">
                <h2>
                    <xsl:value-of select="@id"/>
                </h2>
                <h4>
                    Constant Integer
                </h4>
                <p>
                    <xsl:value-of select="./description"/>
                </p>
                <table height="20pt"/>
            </xsl:for-each>
        </blockquote>
        <blockquote>
            <xsl:for-each select="float">
                <h2>
                    <xsl:value-of select="@id"/>
                </h2>
                <h4>
                    Constant Float (C++ double)
                </h4>
                <p>
                    <xsl:value-of select="./description"/>
                </p>
                <table height="20pt"/>
            </xsl:for-each>
        </blockquote>
        <blockquote>
            <xsl:for-each select="block">
                <h2>
                    <xsl:value-of select="@id"/>
                </h2>
                <h4>
                    Block Of Symbols
                </h4>
                <p>
                    <xsl:value-of select="./description"/>
                </p>
                <font size="-1">
                    <!-- xsl:call-template name="do_block" select="block"/ -->
                    <table height="20pt"/>
                </font>
            </xsl:for-each>
        </blockquote>
        <hr/>
    </xsl:template>


</xsl:stylesheet>
