void setup() {
  // put your setup code here, to run once:

}

void loop() {
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  printf("Chip model %d, revision %d, #cores %d\n",
         chip_info.model, chip_info.revision, chip_info.cores);
  uint32_t free_heap_size = esp_get_free_heap_size();
  uint32_t min_free_heap_size = esp_get_minimum_free_heap_size();
  printf("free heap: %d, min_free_heap: %d\n",
         free_heap_size, min_free_heap_size);
  delay(1000);
}
