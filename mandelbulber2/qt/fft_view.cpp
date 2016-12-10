/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cFFTView - promoted QWidget for displaying of fft audio course
 */

#include "fft_view.h"

#include <QPainter>
#include <QtGui>

#include "../src/audio_track.h"
#include "../src/audio_fft_data.h"
#include "../src/system.hpp"

cFFTView::cFFTView(QWidget *parent) : QWidget(parent)
{
	numberOfFrames = 0;
	framesPerSecond = 30.0;
}

cFFTView::~cFFTView()
{
	// TODO Auto-generated destructor stub
}

void cFFTView::SetParameters(double _framesPerSecond)
{
	framesPerSecond = _framesPerSecond;
}

void cFFTView::AssignAudioTrack(const cAudioTrack *audiotrack)
{

	if (audiotrack)
	{
		int numberOfSampels = audiotrack->getLength();
		int sampleRate = audiotrack->getSampleRate();
		int numberOfFrames = numberOfSampels * framesPerSecond / sampleRate;

		this->setFixedWidth(numberOfFrames);

		const int height = 256;

		fftImage = QImage(QSize(numberOfFrames, height), QImage::Format_RGB32);

		for (int x = 0; x < numberOfFrames; x++)
		{
			cAudioFFTdata fftFrame = audiotrack->getFFTSample(x);
			for (int y = 0; y < height; y++)
			{
				int y2 = height - y - 1;
				double value = fftFrame.data[y] * 0.1;

				QRgb pixel;
				if (value < 0.5)
				{
					pixel = qRgba(0, value * 510, 0, 255);
				}
				else
				{
					if (value > 1.0) value = 1.0;
					pixel = qRgba((value - 0.5) * 510, 255, 0, 255);
				}

				QRgb *line = (QRgb *)fftImage.scanLine(y2);
				line[x] = pixel;
			}
		}

		QPainter painter(&fftImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		scaledFftImage =
			fftImage.scaled(this->width(), height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		update();
		WriteLog("FFTView created", 2);
	}
}

void cFFTView::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.drawImage(0, 0, scaledFftImage);
}