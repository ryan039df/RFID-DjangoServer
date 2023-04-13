from django.db import models

class RFID(models.Model):
    value = models.CharField(max_length=200)
    reader_id = models.CharField(max_length=200)
    time = models.DateTimeField(auto_now_add="True")

    def __str__(self) -> str:
        return self.value[:]