/**
 * the package id used to optimize the dom tree
 */
package org.joy.analyzer.html;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import org.xml.sax.InputSource;
import org.cyberneko.html.parsers.DOMParser;
import org.joy.analyzer.Paragraph;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * @version 1.0
 * @author JINJUN
 * @modified by Liu Song
 * <pre>
 *      这个类将所有的正文文本从一篇HTML文档中提取出来，并且分成若干段落。
 *      @note 段落 是指一篇HTML文档中，语义相对集中的一段文字。
 *      每一个提取出来的段落将会获得一个权重。这个权重是该段落对HTML文档主题的贡献程度
 *
 *      Usage:
 *      BufferedReader reader = new BufferedReader(new FileReader(...));
 *		DOMParser parser = new DOMParser();
 *		parser.parse(new InputSource(reader));
 *		Document doc = parser.getDocument();

 *		TextExtractor extractor = new TextExtractor(doc);
 *  	String text = extractor.extract();
 *  	System.out.println(text);
 *  </pre>
 */
public class TextExtractor {

    /**
     * 这个标记最大允许的锚文本密度，该值目前为.5
     */
    public static final double MAX_ANCHOR_DEN = 0.5;
    /**
     * 该HTML文档中有多少正文文字？
     */
    private int totalTextLen = 0;
    /**
     * 该HTML文档中有多少超链接文字？
     */
    private int totalAnchorTextLen = 0;
    /**
     * 该HTML文档中有多少infoNodes?
     */
    private int totalNumInfoNodes = 0;
    /**
     * 标记列表
     */
    private List<Tag> tagList = new ArrayList<Tag>();
    /**
     * 段落列表
     */
    private List<Paragraph> paragraphList = new ArrayList<Paragraph>();
    /**
     * w3cHTML文档模型
     */
    private Document doc;

    /**
     * 利用給定的W3C文檔對象模型構造一個TextExtractor
     * @param doc 所给定的W3C文档对象模型
     */
    public TextExtractor(Document doc) {
        super();
        this.doc = doc;

    }

    /**
     * 删除文档中的一些显然不会包含主题信息的节点，例如script,style,等等，它们将影响我们的文本抽取器的分析。
     * @param e 所需要清楚地w3c节点
     */
    private void cleanup(Element e) {
        NodeList c = e.getChildNodes();
        for (int i = 0; i < c.getLength(); i++) {
            if (c.item(i).getNodeType() == Node.ELEMENT_NODE) {
                Element t = (Element) c.item(i);
                if (Utility.isInvalidElement(t)) {
                    e.removeChild(c.item(i));
                } else {
                    cleanup(t);
                }
            }
        }
    }

    /**
     * 这个算法目前还不被使用，因为它将会扰乱评分系统的公平性。
     * @deprecated 它将会扰乱评分系统的公平性。
     * @param e
     */
    private void adjust(Element e) {
        NodeList c = e.getChildNodes();
        for (int i = 0; i < c.getLength(); i++) {
            if (c.item(i).getNodeType() == Node.ELEMENT_NODE) {
                Tag tag = new Tag((Element) c.item(i));
                if (Utility.isLargeNode((Element) c.item(i)) &&
                        tag.weight(totalTextLen, totalAnchorTextLen, totalNumInfoNodes) > 0.5) {
                    e.removeChild(c.item(i));
                } else {
                    cleanup((Element) c.item(i));
                }
            }
        }
    }

    /**
     * 抽取HTML文本信息，并且分段，为每一段文本的主题相关性打分。
     * @return 所抽取出的主题信息
     */
    public String extract() {
        long s = System.currentTimeMillis();
        Node body = doc.getElementsByTagName("BODY").item(0);
        //cleanup, remove the invalid tags,
        cleanup((Element) body);
        
        String whole = Tag.getInnerText(body, true);
        totalTextLen = Tag.getInnerText(body, false).length();
        // get anchor text length
        totalAnchorTextLen = Tag.getAnchorText((Element) body).length();

        totalNumInfoNodes = Tag.getNumInfoNode((Element) body);

        extractTags(body);

        String bodyText = "";
        if (tagList.size() == 0) {
            bodyText = "";
        } else {
            //get the max score
            Collections.sort(tagList, new Comparator<Tag>() {

                public int compare(Tag t1, Tag t2) {
                    if (t1.weight(totalTextLen, totalAnchorTextLen, totalNumInfoNodes) >
                            t2.weight(totalTextLen, totalAnchorTextLen, totalNumInfoNodes)) {
                        return 1;
                    } else {
                        return -1;
                    }
                }
            });
            Tag max = tagList.get(tagList.size() - 1);
//            for (Tag t : tagList) {
//                System.out.println(t.getInnerText(false) + "\t" + t.getWeight());
//            }
            //print the method excution duration
            System.out.print("\t" + (System.currentTimeMillis() - s) + "\t");
            //adjust((Element) max.node);
            bodyText = max.getInnerText(true);

        }
        if (!bodyText.trim().equals("")) {
            //extract all the paragraphs, add them to the paragraph list
            paragraphList = new ParagraphSplitter(bodyText, whole).split();
        }
        return bodyText;
    }

    /**
     * 遍历每个Node对象，把每个Node都存储到taglist当中。
     * @param node 所需要遍历的w3cNode对象
     */
    private void extractTags(Node node) {
        if (node.getNodeType() == Node.TEXT_NODE) {
            return;
        }

        if (node.getNodeType() == Node.ELEMENT_NODE) {
            Element element = (Element) node;
            if (Utility.isInvalidElement(element)) {
                return;
            }
            //add the tags
            tagList.add(new Tag(node));
            NodeList list = element.getChildNodes();
            for (int i = 0; i < list.getLength(); i++) {
                extractTags(list.item(i));
            }
        }
    }

    /**
     * 段落列表
     * @return 段落列表
     */
    public List<Paragraph> getParagraphList() {
        return paragraphList;
    }

    /**
     * Test method
     * @param args
     * @throws Exception
     */
    public static void main(String[] args) throws Exception {
        File folder = new File("d:/res2/");
        for (File f : folder.listFiles()) {
            DOMParser parser = new DOMParser();
            BufferedReader reader = new BufferedReader(new FileReader("d://res2/" + f.getName()));
            parser.parse(new InputSource(reader));
            Document doc = parser.getDocument();

            System.out.print(f.getName() + "....");

            TextExtractor extractor = new TextExtractor(doc);
            String str = extractor.extract();
            StringBuffer sb = new StringBuffer();
            for (Paragraph p : extractor.getParagraphList()) {
                sb.append(p.getText() + "\r\n" + p.getWeight() + "\r\n");
            //System.out.print(p.getWeight()+"...\n");
            }
            System.out.println(extractor.totalAnchorTextLen + ":  " + extractor.totalTextLen + "\t" +
                    extractor.totalAnchorTextLen / (float) extractor.totalTextLen);

            //System.out.println(str);
            FileWriter fw = new FileWriter("d:/res3/" + f.getName() + ".txt");
            fw.write(sb.toString());
            fw.close();
        }
    }
}
