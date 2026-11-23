#include "polish_course.h"

// Card indices are stable audio IDs. Append new lessons after the existing ones.
static const polish_card_t cards[] = {
    // Greetings
    {"Cześć", "你好", "ˈtʂɛɕtɕ"},
    {"Dzień dobry", "您好（白天问候）", "dʑɛɲ ˈdɔbrɨ"},
    {"Dobry wieczór", "晚上好", "ˈdɔbrɨ ˈvjɛtʂur"},
    {"Dobranoc", "晚安", "dɔˈbranɔts"},
    {"Do widzenia", "再见", "dɔ viˈdzɛɲa"},
    {"Pa pa", "拜拜", "ˈpa ˈpa"},
    {"Dziękuję", "谢谢", "dʑɛŋˈkujɛ"},
    {"Bardzo dziękuję", "非常感谢", "ˈbardzɔ dʑɛŋˈkujɛ"},
    {"Przepraszam", "对不起", "pʂɛˈpraʂam"},
    {"Proszę", "请", "ˈprɔʂɛ"},
    {"Tak", "是", "ˈtak"},
    {"Nie", "不是", "ˈɲɛ"},
    {"Dobrze", "好", "ˈdɔbʐɛ"},
    {"Źle", "不好", "ˈʑlɛ"},
    {"Jak się masz?", "你好吗", "jak ɕɛ maʂ"},

    // Numbers
    {"zero", "零", "ˈzɛrɔ"},
    {"jeden", "一", "ˈjɛdɛn"},
    {"dwa", "二", "ˈdva"},
    {"trzy", "三", "ˈtʂɨ"},
    {"cztery", "四", "ˈtʂtɛrɨ"},
    {"pięć", "五", "ˈpjɛɲtɕ"},
    {"sześć", "六", "ˈʂɛɕtɕ"},
    {"siedem", "七", "ˈɕɛdɛm"},
    {"osiem", "八", "ˈɔɕɛm"},
    {"dziewięć", "九", "ˈdʑɛvjɛɲtɕ"},
    {"dziesięć", "十", "ˈdʑɛɕɛɲtɕ"},
    {"jedenaście", "十一", "jɛdɛˈnaɕtɕɛ"},
    {"dwanaście", "十二", "dvaˈnaɕtɕɛ"},
    {"trzynaście", "十三", "tʂɨˈnaɕtɕɛ"},
    {"czternaście", "十四", "tʂtɛrˈnaɕtɕɛ"},
    {"piętnaście", "十五", "pjɛtˈnaɕtɕɛ"},
    {"szesnaście", "十六", "ʂɛsˈnaɕtɕɛ"},
    {"siedemnaście", "十七", "ɕɛdɛmˈnaɕtɕɛ"},
    {"osiemnaście", "十八", "ɔɕɛmˈnaɕtɕɛ"},
    {"dziewiętnaście", "十九", "dʑɛvjɛtˈnaɕtɕɛ"},
    {"dwadzieścia", "二十", "dvaˈdʑɛɕtɕa"},
    {"trzydzieści", "三十", "tʂɨˈdʑɛɕtɕi"},
    {"czterdzieści", "四十", "tʂtɛrˈdʑɛɕtɕi"},
    {"pięćdziesiąt", "五十", "pjɛɲˈdʑɛɕɔnt"},
    {"sto", "一百", "ˈstɔ"},
    {"tysiąc", "一千", "ˈtɨɕɔnts"},

    // Colours
    {"czerwony", "红色", "tʂɛrˈvɔnɨ"},
    {"niebieski", "蓝色", "ɲɛˈbjɛski"},
    {"zielony", "绿色", "ʑɛˈlɔnɨ"},
    {"żółty", "黄色", "ˈʐuwtɨ"},
    {"biały", "白色", "ˈbjawɨ"},
    {"czarny", "黑色", "ˈtʂarnɨ"},
    {"szary", "灰色", "ˈʂarɨ"},
    {"brązowy", "棕色", "brɔ̃ˈzɔvɨ"},
    {"różowy", "粉色", "ruˈʐɔvɨ"},
    {"pomarańczowy", "橙色", "pɔmaraɲˈtʂɔvɨ"},
    {"fioletowy", "紫色", "fjɔlɛˈtɔvɨ"},
    {"złoty", "金色", "ˈzwɔtɨ"},
    {"srebrny", "银色", "ˈsrɛbrnɨ"},
    {"jasny", "浅色的", "ˈjasnɨ"},
    {"ciemny", "深色的", "ˈtɕɛmnɨ"},

    // Food and drink
    {"woda", "水", "ˈvɔda"},
    {"chleb", "面包", "ˈxlɛp"},
    {"mleko", "牛奶", "ˈmlɛkɔ"},
    {"kawa", "咖啡", "ˈkava"},
    {"herbata", "茶", "xɛrˈbata"},
    {"mięso", "肉", "ˈmjɛ̃sɔ"},
    {"ryba", "鱼", "ˈrɨba"},
    {"jajko", "鸡蛋", "ˈjajkɔ"},
    {"ser", "奶酪", "ˈsɛr"},
    {"masło", "黄油", "ˈmaswɔ"},
    {"cukier", "糖", "ˈtsukjɛr"},
    {"sól", "盐", "ˈsul"},
    {"jabłko", "苹果", "ˈjapkɔ"},
    {"banan", "香蕉", "ˈbanan"},
    {"pomidor", "番茄", "pɔˈmidɔr"},
    {"ziemniak", "土豆", "ˈʑɛmɲak"},
    {"piwo", "啤酒", "ˈpivɔ"},
    {"wino", "葡萄酒", "ˈvinɔ"},
    {"śniadanie", "早餐", "ɕɲaˈdaɲɛ"},
    {"obiad", "午餐", "ˈɔbjat"},
    {"kolacja", "晚餐", "kɔˈlatsja"},

    // Family
    {"mama", "妈妈", "ˈmama"},
    {"tata", "爸爸", "ˈtata"},
    {"brat", "哥哥或弟弟", "ˈbrat"},
    {"siostra", "姐姐或妹妹", "ˈɕɔstra"},
    {"syn", "儿子", "ˈsɨn"},
    {"córka", "女儿", "ˈtsurka"},
    {"babcia", "奶奶或外婆", "ˈbaptɕa"},
    {"dziadek", "爷爷或外公", "ˈdʑadɛk"},
    {"wujek", "叔叔或舅舅", "ˈvujɛk"},
    {"ciotka", "姑姑或姨妈", "ˈtɕɔtka"},
    {"mąż", "丈夫", "ˈmɔ̃ʂ"},
    {"żona", "妻子", "ˈʐɔna"},
    {"dziecko", "孩子", "ˈdʑɛtskɔ"},
    {"rodzina", "家庭", "rɔˈdʑina"},
    {"przyjaciel", "男性好友", "pʂɨˈjatɕɛl"},

    // Verbs
    {"być", "是", "ˈbɨtɕ"},
    {"mieć", "有", "ˈmjɛtɕ"},
    {"iść", "走", "ˈiɕtɕ"},
    {"jeść", "吃", "ˈjɛɕtɕ"},
    {"pić", "喝", "ˈpitɕ"},
    {"spać", "睡觉", "ˈspatɕ"},
    {"mówić", "说", "ˈmuvitɕ"},
    {"czytać", "读", "ˈtʂɨtatɕ"},
    {"pisać", "写", "ˈpisatɕ"},
    {"pracować", "工作", "praˈtsɔvatɕ"},
    {"uczyć się", "学习", "ˈutʂɨtɕ ɕɛ"},
    {"kochać", "爱", "ˈkɔxatɕ"},
    {"chcieć", "想要", "ˈxtɕɛtɕ"},
    {"móc", "能够", "ˈmuts"},
    {"wiedzieć", "知道", "ˈvjɛdʑɛtɕ"},

    // Useful phrases
    {"Ile to kosztuje?", "多少钱", "ˈilɛ ˈtɔ kɔʂˈtujɛ"},
    {"Gdzie jest...?", "在哪里", "ˈgdʑɛ ˈjɛst"},
    {"Jak się nazywasz?", "你叫什么", "jak ɕɛ naˈzɨvaʂ"},
    {"Nazywam się...", "我的名字是", "naˈzɨvam ɕɛ"},
    {"Nie rozumiem", "不明白", "ˈɲɛ rɔˈzumjɛm"},
    {"Rozumiem", "明白了", "rɔˈzumjɛm"},
    {"Mów wolniej", "说慢点", "ˈmuv ˈvɔlɲɛj"},
    {"Gdzie toaleta?", "厕所在哪", "ˈgdʑɛ tɔaˈlɛta"},
    {"Pomocy!", "救命", "pɔˈmɔtsɨ"},
    {"Smacznego", "祝你用餐愉快", "smatʂˈnɛɡɔ"},
    {"Na zdrowie", "干杯", "na ˈzdrɔvjɛ"},
    {"Wszystkiego dobrego", "祝你一切顺利", "fʂɨstˈkjɛɡɔ dɔˈbrɛɡɔ"},
    {"Miło mi", "很高兴认识你", "ˈmiwɔ ˈmi"},
    {"Nie ma sprawy", "没问题", "ɲɛ ma ˈspravɨ"},
    {"Co słychać?", "最近怎么样", "ˈtsɔ ˈswɨxatɕ"},

    // At a café: order, choose milk and takeaway, then ask for the bill.
    {"Czy jest wolny stolik?", "有空桌吗", "tʂɨ jɛst ˈvɔlnɨ ˈstɔlik"},
    {"Dla dwóch osób.", "两位用餐", "dla dvux ˈɔsup"},
    {"Poproszę kawę.", "请给我一杯咖啡", "pɔˈprɔʂɛ ˈkavɛ"},
    {"Z mlekiem czy bez?", "要加奶还是不加", "z ˈmlɛkjɛm tʂɨ bɛs"},
    {"Bez mleka.", "不加奶", "bɛz ˈmlɛka"},
    {"Na miejscu czy na wynos?", "堂食还是外带", "na ˈmjɛjstsu tʂɨ na ˈvɨnɔs"},
    {"Na wynos.", "外带", "na ˈvɨnɔs"},
    {"Poproszę rachunek.", "请给我账单", "pɔˈprɔʂɛ raˈxunɛk"},

    // At a shop: ask the price, specify a quantity and pay.
    {"Szukam chleba.", "我在找面包", "ˈʂukam ˈxlɛba"},
    {"Ile kosztuje kilo?", "一公斤多少钱", "ˈilɛ kɔʂˈtujɛ ˈkilɔ"},
    {"Dwa złote.", "两兹罗提", "dva ˈzwɔtɛ"},
    {"Poproszę dwa banany.", "请给我两根香蕉", "pɔˈprɔʂɛ dva baˈnanɨ"},
    {"Czy mogę zapłacić kartą?", "我能刷卡吗", "tʂɨ ˈmɔɡɛ zaˈpwatɕitɕ ˈkartɔ̃"},
    {"Mam gotówkę.", "我有现金", "mam ɡɔˈtufkɛ"},
    {"To wszystko.", "就这些", "tɔ ˈfʂɨstkɔ"},
    {"Poproszę paragon.", "请给我小票", "pɔˈprɔʂɛ paˈraɡɔn"},

    // Directions: ask, then follow short instructions.
    {"Gdzie jest bank?", "银行在哪里", "ɡdʑɛ jɛzd baŋk"},
    {"To daleko?", "远吗", "tɔ daˈlɛkɔ"},
    {"To blisko.", "很近", "tɔ ˈbliskɔ"},
    {"Proszę iść prosto.", "请直走", "ˈprɔʂɛ iɕtɕ ˈprɔstɔ"},
    {"W lewo.", "向左", "v ˈlɛvɔ"},
    {"W prawo.", "向右", "f ˈpravɔ"},
    {"Za rogiem.", "在拐角那边", "za ˈrɔɡjɛm"},
    {"Po lewej stronie.", "在左侧", "pɔ ˈlɛvɛj ˈstrɔɲɛ"},

    // Transport: read station signs, buy a ticket and find the platform.
    {"przystanek", "乘车站点", "pʂɨˈstanɛk"},
    {"bilet", "票", "ˈbilɛt"},
    {"pociąg", "火车", "ˈpɔtɕɔŋk"},
    {"peron", "站台", "ˈpɛrɔn"},
    {"Bilet do Krakowa.", "一张去克拉科夫的票", "ˈbilɛd dɔ kraˈkɔva"},
    {"W jedną stronę.", "单程", "v ˈjɛdnɔ̃ ˈstrɔnɛ"},
    {"Z którego peronu?", "从哪个站台出发", "s ktuˈrɛɡɔ pɛˈrɔnu"},
    {"Z peronu drugiego.", "从二号站台出发", "s pɛˈrɔnu druˈɡjɛɡɔ"},

    // Time: distinguish telling the time from arranging a meeting.
    {"dzisiaj", "今天", "ˈdʑiɕaj"},
    {"jutro", "明天", "ˈjutrɔ"},
    {"rano", "早晨", "ˈranɔ"},
    {"wieczorem", "在晚上", "vjɛˈtʂɔrɛm"},
    {"Która godzina?", "现在几点", "ˈktura ɡɔˈdʑina"},
    {"Jest ósma.", "现在八点", "jɛst ˈusma"},
    {"O której godzinie?", "在几点钟", "ɔ ˈkturɛj ɡɔˈdʑiɲɛ"},
    {"O ósmej rano.", "在早上八点", "ɔ ˈusmɛj ˈranɔ"},

    // At a hotel: check in, give the length of stay and locate the room.
    {"pokój", "房间", "ˈpɔkuj"},
    {"klucz", "钥匙", "klutʂ"},
    {"Mam rezerwację.", "我有预订", "mam rɛzɛrˈvatsjɛ"},
    {"Na ile nocy?", "要住几晚", "na ˈilɛ ˈnɔtsɨ"},
    {"Na dwie noce.", "住两晚", "na dvjɛ ˈnɔtsɛ"},
    {"Czy jest winda?", "有电梯吗", "tʂɨ jɛst ˈvinda"},
    {"Gdzie jest mój pokój?", "我的房间在哪里", "ɡdʑɛ jɛst muj ˈpɔkuj"},
    {"Na drugim piętrze.", "在地面层往上两层", "na ˈdruɡim ˈpjɛntʂɛ"},

    // Introductions: pair questions with complete sample answers.
    {"Jak masz na imię?", "你叫什么名字", "jak maʂ na ˈimjɛ"},
    {"Mam na imię Anna.", "我的名字是安娜", "mam na ˈimjɛ ˈanna"},
    {"Skąd jesteś?", "你从哪里来", "skɔnt ˈjɛstɛɕ"},
    {"Jestem z Chin.", "我来自中国", "ˈjɛstɛm s xin"},
    {"Gdzie mieszkasz?", "你住在哪里", "ɡdʑɛ ˈmjɛʂkaʂ"},
    {"Mieszkam w Polsce.", "我住在波兰", "ˈmjɛʂkam f ˈpɔlstsɛ"},
    {"Mówię po chińsku.", "我会说中文", "ˈmuvjɛ pɔ ˈxiɲsku"},
    {"Uczę się polskiego.", "我在学波兰语", "ˈutʂɛ ɕɛ pɔlˈskjɛɡɔ"},

    // Sentence patterns: compare persons, questions and negation in context.
    {"Mam czas.", "我有时间", "mam tʂas"},
    {"Masz czas?", "你有时间吗", "maʂ tʂas"},
    {"Nie mam czasu.", "我没有时间", "ɲɛ mam ˈtʂasu"},
    {"Czytam książkę.", "我在读书", "ˈtʂɨtam ˈkɕɔ̃ʂkɛ"},
    {"Co czytasz?", "你在读什么", "tsɔ ˈtʂɨtaʂ"},
    {"Lubię kawę.", "我喜欢咖啡", "ˈlubjɛ ˈkavɛ"},
    {"Lubisz kawę?", "你喜欢咖啡吗", "ˈlubiʂ ˈkavɛ"},
    {"Nie lubię kawy.", "我不喜欢咖啡", "ɲɛ ˈlubjɛ ˈkavɨ"},
};

typedef struct {
    const char *name;
    const char *key;
    size_t first;
} topic_t;

// Each topic runs up to the start of the next topic (or the end of the deck).
static const topic_t topics[] = {
    {"问候", "greetings", 0},
    {"数字", "numbers", 15},
    {"颜色", "colors", 41},
    {"食物", "food", 56},
    {"家庭", "family", 77},
    {"动词", "verbs", 92},
    {"短语", "phrases", 107},
    {"咖啡馆", "cafe", 122},
    {"购物", "shopping", 130},
    {"问路", "directions", 138},
    {"乘车", "transport", 146},
    {"时间", "time", 154},
    {"住宿", "lodging", 162},
    {"自我介绍", "introductions", 170},
    {"日常句型", "patterns", 178},
};

size_t polish_card_count(void) {
    return sizeof(cards) / sizeof(cards[0]);
}

size_t polish_topic_count(void) {
    return sizeof(topics) / sizeof(topics[0]);
}

const char *polish_topic_name(size_t topic) {
    return topic < polish_topic_count() ? topics[topic].name : NULL;
}

const char *polish_topic_key(size_t topic) {
    return topic < polish_topic_count() ? topics[topic].key : NULL;
}

size_t polish_topic_first(size_t topic) {
    return topic < polish_topic_count() ? topics[topic].first : polish_card_count();
}

size_t polish_topic_size(size_t topic) {
    if (topic >= polish_topic_count()) return 0;
    return polish_topic_first(topic + 1) - topics[topic].first;
}

const polish_card_t *polish_card_at(size_t global_index) {
    return global_index < polish_card_count() ? &cards[global_index] : NULL;
}
