#include "includes/live_camera.h"

#include <sstream>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include <QTimer>
#include <QVideoSink>
#include <QVideoFrame>
#include <QImage>
#include <QDebug>

using namespace std;

string LiveCamera::replaceAll(string str, const string &remove, const string &insert) {
    string::size_type pos = 0;
    while ((pos = str.find(remove, pos)) != string::npos) {
        str.replace(pos, remove.size(), insert);
        pos++;
    }

    return str;
}

string LiveCamera::trimString(string str) {
    string stripString = str;
    while(!stripString.empty() && isspace(*stripString.begin()))
        stripString.erase(stripString.begin());

    while(!stripString.empty() && isspace(*stripString.rbegin()))
        stripString.erase(stripString.length()-1);

    return stripString;
}

// Get and Populate CameraInfo to CameraList
void LiveCamera::liveCamera_get_camera_info(map<string, map<string,string>> &cameraInfo)
{
    QStringList list = LiveCamera_list.stringList();
    // Get camera info from setup_camera script
    string command = "bash /opt/ti-apps-launcher/setup_cameras.sh";
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    vector<string> split_string{};
    auto ss = stringstream{result};
    unsigned int i,j;

    for (string line; getline(ss, line, '\n');) {
        qDebug() << QString::fromStdString(line);
        split_string.push_back(line);
    }

    for(i = 0; i < split_string.size(); i++) {
        if (split_string[i].find("detected") != string::npos) {
            string cameraName = replaceAll(split_string[i],"detected","");
            cameraName = trimString(cameraName);

            map<string, string> info{};
            for (j = i+1; j < split_string.size(); j++) {
                if (split_string[j].find("detected") != string::npos) {
                    break;
                }

                char *token;
                token = strtok(&split_string[j][0], "=");
                string key = trimString(token);
                token =  strtok(NULL, "=");
                string value = trimString(token);
                info[key] = value;
            }
            if (info.size() > 0) {
                cameraInfo[cameraName] = info;
            }
        }
    }
    for ( const auto &data : cameraInfo ) {
        for ( const auto &detailedData : data.second )
        {
            if (detailedData.first.find("device") != string::npos)
            {
                string fullName,device;
                device = replaceAll(detailedData.first,"device","");
                device = trimString(device);
                fullName = data.first;
                if (device.length() > 0)
                    fullName += " " + device;
                list.append(QString::fromStdString(fullName));
            }
        }
    }
    LiveCamera_list.setStringList(list);

    LiveCamera_count = LiveCamera_list.rowCount();
}

void LiveCamera::liveCamera_update_gst_pipeline(QString camera) {
    _camera = camera;
}

QString LiveCamera::liveCamera_get_camera_name(int index) {
    return LiveCamera_list.stringList().at(index);
}

QString LiveCamera::liveCamera_gst_pipeline() {
    gst_pipeline = "gst-pipeline: ";
    gst_pipeline.append("v4l2src device=");
    gst_pipeline.append(QString::fromStdString((cameraInfo[_camera.toStdString()]["device"])));
    #if defined(SOC_AM62) || defined(SOC_AM62_LP) || defined(SOC_AM62P)
    if (_camera.contains("USB", Qt::CaseInsensitive)) {
        gst_pipeline.append(" ! video/x-raw, width=640, height=480, format=YUY2");
    } else {
        gst_pipeline.append(" ! video/x-raw, width=640, height=480, format=UYVY");
    }
    #elif defined(SOC_J721E) || defined(SOC_J721S2) || defined(SOC_J784S4) || defined(SOC_J722S)
    gst_pipeline.append(" ! image/jpeg, width=1280, height=720 ! jpegdec");
    #endif
    gst_pipeline.append(" ! videoconvert");
    gst_pipeline.append(" ! appsink");
    qDebug() << gst_pipeline;
    return gst_pipeline;
}

int LiveCamera::liveCamera_get_count() {
    cameraInfo.clear();
    LiveCamera_count = 0;
    for (int i = LiveCamera_list.rowCount(); i > 0; i--) {
        LiveCamera_list.removeRow(--i);
    }
    liveCamera_get_camera_info(cameraInfo);
    return LiveCamera_count;
}

void LiveCamera::startStream()
{
	QString gst_pipeline = liveCamera_gst_pipeline();
	GError *error = NULL;
	m_pipeline = gst_parse_launch(gst_pipeline.toLatin1().data(), &error);
	if (error) {
		g_printerr("Failed to parse launch: %s\n", error->message);
		g_error_free(error);
		return;
	}

	GstElement *appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), "sink");
    g_object_set(G_OBJECT(appsink), "emit-signals", TRUE, nullptr);
    g_signal_connect(appsink, "new-sample", G_CALLBACK(onNewSample), this);
    gst_object_unref(appsink);

	qDebug() << "Starting stream";
	gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
}

void LiveCamera::stopStream()
{
	if (m_pipeline) {
		qDebug() << "Stopping camera stream";
		gst_element_set_state(m_pipeline, GST_STATE_NULL);
		qDebug() << "Removing pipeline";
		gst_object_unref(m_pipeline);
		m_pipeline = NULL;
	}
}

QVideoSink* LiveCamera::videoSink()
{
	return m_videoSink;
}

static GstFlowReturn onNewSample(GstAppSink *appsink, gpointer user_data) {
	CameraCapture *self = static_cast<CameraCapture *>(user_data);
    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if (!sample) return GST_FLOW_OK;

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if (!gst_buffer_map(buffer, &map, GST_MAP_READ)) {
		gst_sample_unref(sample);
		return GST_FLOW_OK;
	}

    int width = 640;
    int height = 480;
    QImage img(map.data, width, height, QImage::Format_RGB888);
    QVideoFrame frame(img);
    if (self->m_videoSink) {
		self->m_videoSink->setVideoFrame(frame);
    }

    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);
    return GST_FLOW_OK;
}
