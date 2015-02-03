static unsigned long davidegrayson_nibble_sort_counter_word(unsigned long w) {
  unsigned char frequency_map[0x10] = {0};
  frequency_map[w >> 0x00 & 0xf]++;
  frequency_map[w >> 0x04 & 0xf]++;
  frequency_map[w >> 0x08 & 0xf]++;
  frequency_map[w >> 0x0c & 0xf]++;
  frequency_map[w >> 0x10 & 0xf]++;
  frequency_map[w >> 0x14 & 0xf]++;
  frequency_map[w >> 0x18 & 0xf]++;
  frequency_map[w >> 0x1c & 0xf]++;
  frequency_map[w >> 0x20 & 0xf]++;
  frequency_map[w >> 0x24 & 0xf]++;
  frequency_map[w >> 0x28 & 0xf]++;
  frequency_map[w >> 0x2c & 0xf]++;
  frequency_map[w >> 0x30 & 0xf]++;
  frequency_map[w >> 0x34 & 0xf]++;
  frequency_map[w >> 0x38 & 0xf]++;
  frequency_map[w >> 0x3c & 0xf]++;

  unsigned long r = 0;
  for (unsigned int j = 0; j < frequency_map[0x1]; j++)
    r = ((unsigned long)0x1 << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0x2]; j++)
    r = ((unsigned long)0x2 << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0x3]; j++)
    r = ((unsigned long)0x3 << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0x4]; j++)
    r = ((unsigned long)0x4 << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0x5]; j++)
    r = ((unsigned long)0x5 << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0x6]; j++)
    r = ((unsigned long)0x6 << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0x7]; j++)
    r = ((unsigned long)0x7 << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0x8]; j++)
    r = ((unsigned long)0x8 << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0x9]; j++)
    r = ((unsigned long)0x9 << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0xa]; j++)
    r = ((unsigned long)0xa << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0xb]; j++)
    r = ((unsigned long)0xb << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0xc]; j++)
    r = ((unsigned long)0xc << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0xd]; j++)
    r = ((unsigned long)0xd << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0xe]; j++)
    r = ((unsigned long)0xe << 60) | (r >> 4);
  for (unsigned int j = 0; j < frequency_map[0xf]; j++)
    r = ((unsigned long)0xf << 60) | (r >> 4);

  return r;
}

void nibble_sort_grayson(unsigned long *buf) {
  for (int i = 0; i < 1024; i++) {
    buf[i] = davidegrayson_nibble_sort_counter_word(buf[i]);
  }
}
