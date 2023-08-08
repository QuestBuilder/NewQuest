void wait(int secs)
{
  long t = millis();
  while(millis() - t < secs * 1000) {;}
}


int calcTSize(String txt, float maxWidth)
{
  int retSize = 1;
  float calcWidth = 0;
  while (calcWidth < maxWidth)
  {
    if (txt.equals("")) txt = "00:00:00";
    timer_font = createFont("Silom", 14);
    textFont(timer_font, retSize);
    calcWidth = textWidth(txt);
    //println(retSize+":"+txt+":"+calcWidth);
    retSize += 1;
  }
  return --retSize;
}

String getTime(int h, int m, int s)
{
  String h_str = str(h);
  if (h < 10) h_str = "0" + str(h);
  String m_str = str(m);
  if (m < 10) m_str = "0" + str(m);
  String s_str = str(s);
  if (s < 10) s_str = "0" + str(s);
  return h_str+":"+m_str+":"+s_str;
}

int seconds(long elpTime) {
  return int((elpTime / 1000) % 60);
}
int minutes(long elpTime) {
  return int((elpTime / (1000*60)) % 60);
}
int hours(long elpTime) {
  return int((elpTime / (1000*60*60)) % 24);
}

String toStr(char[] a)
{
  int s = a.length;
  String ret = "";
  for (int i = 0; i < s; i++)
  {
    ret += a[i];
  }
  return ret;
}
