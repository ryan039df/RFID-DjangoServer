# Generated by Django 4.2 on 2023-04-13 03:27

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('rfid', '0001_initial'),
    ]

    operations = [
        migrations.AlterField(
            model_name='rfid',
            name='time',
            field=models.DateTimeField(auto_now_add=True),
        ),
    ]
