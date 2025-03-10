const char *ignore_words[] = {
    "and", "or", "but", "nor", "for", "yet", "so", "although", "because", "since", "unless", 
    "until", "while", "when", "where", "after", "before", "once", "if", "though", "even though", 
    "in order that", "rather than", "than", "whether", "either", "neither","nor", 
    "not only...but also", "both...and",
    "a", "an", "the",
    "i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "your", "yours", 
    "yourself", "yourselves", "he", "him", "his", "himself", "she", "her", "hers", "herself", 
    "it", "its", "itself", "they", "them", "their", "theirs", "themselves", "this", "that", 
    "these", "those", "who", "whom", "whose", "which", "what",
    "about", "above", "across", "after", "against", "along", "among", "around", "as", "at", 
    "before", "behind", "below", "beneath", "beside", "between", "beyond", "by", "despite", 
    "down", "during", "except", "for", "from", "in", "inside", "into", "like", "near", "of", 
    "off", "on", "onto", "opposite", "out", "outside", "over", "past", "per", "regarding", 
    "round", "since", "through", "throughout", "till", "to", "toward", "towards", "under", 
    "underneath", "unlike", "until", "up", "upon", "versus", "via", "with", "within", "without",
    "am", "is", "are", "was", "were", "be", "been", "being", "can", "could", "do", "does", 
    "did", "have", "has", "had", "having", "may", "might", "must", "shall", "should", "will", "would",
    "no", "not", "never", "none", "nothing", "nowhere", "nobody"
};

int ignore_words_count = sizeof(ignore_words) / sizeof(ignore_words[0]);
