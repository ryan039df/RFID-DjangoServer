from django.urls import path
from .views import SensorDataView

urlpatterns = [
    path('rfid-data/', SensorDataView.as_view())
]