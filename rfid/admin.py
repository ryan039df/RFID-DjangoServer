from django.contrib import admin

from .models import RFID    

@admin.register(RFID)
class RDIDAdmin(admin.ModelAdmin):
    list_display = ('value', 'reader_id', 'time')

