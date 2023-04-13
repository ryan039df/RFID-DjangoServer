from rest_framework import serializers
from .models import RFID

class SensorDataSerializer(serializers.ModelSerializer):
    class Meta:
        model = RFID
        fields = ('value', 'reader_id')
